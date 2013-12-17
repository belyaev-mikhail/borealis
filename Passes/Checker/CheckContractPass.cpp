/*
 * CheckContractPass.cpp
 *
 *  Created on: Feb 27, 2013
 *      Author: ice-phoenix
 */

#include <llvm/Support/InstVisitor.h>

#include "Codegen/intrinsics_manager.h"
#include "Codegen/llvm.h"
#include "Passes/Checker/CheckContractPass.h"
#include "Passes/Checker/CheckManager.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "SMT/MathSAT/Solver.h"
#include "SMT/Z3/Solver.h"
#include "State/PredicateStateBuilder.h"
#include "State/Transformer/AggregateTransformer.h"
#include "State/Transformer/AnnotationMaterializer.h"
#include "State/Transformer/CallSiteInitializer.h"
#include "State/Transformer/ContractTransmogrifier.h"
#include "State/Transformer/Simplifier.h"

namespace borealis {

////////////////////////////////////////////////////////////////////////////////

class CallInstVisitor : public llvm::InstVisitor<CallInstVisitor> {

public:

    CallInstVisitor(CheckContractPass* pass) : pass(pass) {}

    void visitCallInst(llvm::CallInst& CI) {
        auto& im = IntrinsicsManager::getInstance();

        switch (im.getIntrinsicType(CI)) {
        case function_type::INTRINSIC_ANNOTATION:
            checkAnnotation(CI, Annotation::fromIntrinsic(CI)); break;
        case function_type::ACTION_DEFECT:
            checkActionDefect(CI); break;
        default:
            checkContract(CI);
            checkBonds(CI);
            break;
        }
    }

    void checkBonds(llvm::CallInst& CI) {
        auto bonds = pass->FM->getBonds(CI.getCalledFunction());
        if (bonds.empty()) return;

        auto state = pass->PSA->getInstructionState(&CI);
        if (!state) return;

        for (auto& e : bonds) {
            auto bond = e.second.first;
            auto defect = e.second.second;

            auto t = Simplifier(pass->FN) +
                     ContractTransmogrifier(pass->FN) +
                     CallSiteInitializer(CI, pass->FN);
            auto instantiatedBond = bond->map(
                [&t](Predicate::Ptr p) { return t.transform(p); }
            );

            dbgs() << "Checking: " << CI << endl;
            dbgs() << "  Bond: " << endl << instantiatedBond << endl;

            auto fMemId = pass->FM->getMemoryStart(CI.getParent()->getParent());

    #if defined USE_MATHSAT_SOLVER
            MathSAT::ExprFactory ef;
            MathSAT::Solver s(ef, fMemId);
    #else
            Z3::ExprFactory ef;
            Z3::Solver s(ef, fMemId);
    #endif

            dbgs() << "  State: " << endl << state << endl;
            if (s.isViolated(instantiatedBond, state)) {
                pass->DM->addDefect(defect);
            }
        }
    }

    void checkContract(llvm::CallInst& CI) {
        auto contract = pass->FM->getReq(CI, pass->FN);
        if (contract->isEmpty()) return;

        auto state = pass->PSA->getInstructionState(&CI);
        if (!state) return;

        CallSiteInitializer csi(CI, pass->FN);
        auto instantiatedContract = contract->map(
            [&csi](Predicate::Ptr p) { return csi.transform(p); }
        );

        dbgs() << "Checking: " << CI << endl;
        dbgs() << "  Requires: " << endl << instantiatedContract << endl;

        auto fMemId = pass->FM->getMemoryStart(CI.getParent()->getParent());

#if defined USE_MATHSAT_SOLVER
        MathSAT::ExprFactory ef;
        MathSAT::Solver s(ef, fMemId);
#else
        Z3::ExprFactory ef;
        Z3::Solver s(ef, fMemId);
#endif

        dbgs() << "  State: " << endl << state << endl;
        if (s.isViolated(instantiatedContract, state)) {
            pass->DM->addDefect(DefectType::REQ_01, &CI);
        }
    }

    void checkActionDefect(llvm::CallInst& CI) {
        using namespace llvm;

        auto state = pass->PSA->getInstructionState(&CI);
        if (!state) return;

        dbgs() << "Checking: " << CI << endl;
        dbgs() << "  State: " << endl << state << endl;

        auto fMemId = pass->FM->getMemoryStart(CI.getParent()->getParent());

        if (!state->isUnreachableIn(fMemId)) {
            auto* op0 = CI.getArgOperand(0);
            auto* op1 = CI.getArgOperand(1);

            auto defectType = getAsCompileTimeString(op0);
            auto defectMsg = getAsCompileTimeString(op1);

            pass->DM->addDefect(defectType.getOrElse("UNK-99"), &CI);
        }
    }

    void checkAnnotation(llvm::CallInst& CI, Annotation::Ptr A) {
        auto anno = materialize(A, pass->FN, pass->MI);

        auto state = pass->PSA->getInstructionState(&CI);
        if (!state) return;

        if (auto* LA = llvm::dyn_cast<AssertAnnotation>(anno)) {
            auto query = (
                pass->FN.State *
                pass->FN.Predicate->getEqualityPredicate(
                    LA->getTerm(),
                    pass->FN.Term->getTrueTerm()
                    // predicateType(LA)
                )
            )();

            dbgs() << "Checking: " << CI << endl;
            dbgs() << "  Assert: " << endl << query << endl;

            auto fMemId = pass->FM->getMemoryStart(CI.getParent()->getParent());

#if defined USE_MATHSAT_SOLVER
            MathSAT::ExprFactory ef;
            MathSAT::Solver s(ef, fMemId);
#else
            Z3::ExprFactory ef;
            Z3::Solver s(ef, fMemId);
#endif

            dbgs() << "  State: " << endl << state << endl;
            if (s.isViolated(query, state)) {
                pass->DM->addDefect(DefectType::ASR_01, &CI);
            }
        }
    }

    void visitReturnInst(llvm::ReturnInst& RI) {
        auto contract = pass->FM->getEns(RI.getParent()->getParent());
        if (contract->isEmpty()) return;

        auto state = pass->PSA->getInstructionState(&RI);
        if (!state) return;

        dbgs() << "Checking: " << RI << endl;
        dbgs() << "  Ensures: " << endl << contract << endl;

        auto fMemId = pass->FM->getMemoryStart(RI.getParent()->getParent());

#if defined USE_MATHSAT_SOLVER
        MathSAT::ExprFactory ef;
        MathSAT::Solver s(ef, fMemId);
#else
        Z3::ExprFactory ef;
        Z3::Solver s(ef, fMemId);
#endif

        dbgs() << "  State: " << endl << state << endl;
        if (s.isViolated(contract, state)) {
            pass->DM->addDefect(DefectType::ENS_01, &RI);
        }
    }

private:

    CheckContractPass* pass;

};

////////////////////////////////////////////////////////////////////////////////

CheckContractPass::CheckContractPass() : ProxyFunctionPass(ID) {}
CheckContractPass::CheckContractPass(llvm::Pass* pass) : ProxyFunctionPass(ID, pass) {}

void CheckContractPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();

    AUX<CheckManager>::addRequiredTransitive(AU);

    AUX<DefectManager>::addRequiredTransitive(AU);
    AUX<FunctionManager>::addRequiredTransitive(AU);
    AUX<MetaInfoTracker>::addRequiredTransitive(AU);
    AUX<PredicateStateAnalysis>::addRequiredTransitive(AU);
    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
}

bool CheckContractPass::runOnFunction(llvm::Function& F) {

    if (GetAnalysis<CheckManager>::doit(this, F).shouldSkipFunction(&F))
        return false;

    DM = &GetAnalysis<DefectManager>::doit(this, F);
    FM = &GetAnalysis<FunctionManager>::doit(this, F);
    MI = &GetAnalysis<MetaInfoTracker>::doit(this, F);
    PSA = &GetAnalysis<PredicateStateAnalysis>::doit(this, F);

    auto* st = GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(F);
    FN = FactoryNest(st);

    CallInstVisitor civ(this);
    civ.visit(F);

    return false;
}

CheckContractPass::~CheckContractPass() {}

char CheckContractPass::ID;
static RegisterPass<CheckContractPass>
X("check-contracts", "Pass that checks annotated code contracts");

} /* namespace borealis */
