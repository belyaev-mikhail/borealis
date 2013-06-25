/*
 * CheckContractPass.cpp
 *
 *  Created on: Feb 27, 2013
 *      Author: ice-phoenix
 */

#include <llvm/Support/InstVisitor.h>

#include "Codegen/intrinsics_manager.h"
#include "Logging/logger.hpp"
#include "Passes/Checker/CheckContractPass.h"
#include "Solver/Z3Solver.h"
#include "State/PredicateStateBuilder.h"
#include "State/Transformer/AnnotationMaterializer.h"
#include "State/Transformer/CallSiteInitializer.h"

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
        default:
            checkContract(CI); break;
        }
    }

    void checkContract(llvm::CallInst& CI) {
        auto contract = pass->FM->get(CI, pass->PF.get(), pass->TF.get());
        auto state = pass->PSA->getInstructionState(&CI);

        if (!state) return;

        auto requires = contract->filterByTypes({PredicateType::REQUIRES});
        if (requires->isEmpty()) return;

        CallSiteInitializer csi(CI, pass->TF.get());
        auto instantiatedRequires = requires->map(
            [&csi](Predicate::Ptr p) { return csi.transform(p); }
        );

        dbgs() << "Checking: " << CI << endl;
        dbgs() << "  Requires: " << endl << instantiatedRequires << endl;

        Z3ExprFactory z3ef;
        Z3Solver s(z3ef);

        dbgs() << "  State: " << endl << state << endl;
        if (s.isViolated(instantiatedRequires, state)) {
            pass->DM->addDefect(DefectType::REQ_01, &CI);
        }
    }

    void checkAnnotation(llvm::CallInst& CI, Annotation::Ptr A) {
        auto anno = materialize(A, pass->TF.get(), pass->MI);
        auto state = pass->PSA->getInstructionState(&CI);

        if (!state) return;

        if (auto* LA = llvm::dyn_cast<AssertAnnotation>(anno)) {
            auto query = (
                pass->PSF *
                pass->PF->getEqualityPredicate(
                    LA->getTerm(),
                    pass->TF->getTrueTerm(),
                    predicateType(LA)
                )
            )();

            dbgs() << "Checking: " << CI << endl;
            dbgs() << "  Assert: " << endl << LA << endl;

            Z3ExprFactory z3ef;
            Z3Solver s(z3ef);

            dbgs() << "  State: " << endl << state << endl;
            if (s.isViolated(query, state)) {
                pass->DM->addDefect(DefectType::ASR_01, &CI);
            }
        }
    }

    void visitReturnInst(llvm::ReturnInst& RI) {
        auto contract = pass->FM->get(RI.getParent()->getParent());
        auto state = pass->PSA->getInstructionState(&RI);

        if (!state) return;

        auto ensures = contract->filterByTypes({PredicateType::ENSURES});
        if (ensures->isEmpty()) return;

        dbgs() << "Checking: " << RI << endl;
        dbgs() << "  Ensures: " << endl << ensures << endl;

        Z3ExprFactory z3ef;
        Z3Solver s(z3ef);

        dbgs() << "  State: " << endl << state << endl;
        if (s.isViolated(ensures, state)) {
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

    AUX<DefectManager>::addRequiredTransitive(AU);
    AUX<FunctionManager>::addRequiredTransitive(AU);
    AUX<MetaInfoTrackerPass>::addRequiredTransitive(AU);
    AUX<PredicateStateAnalysis>::addRequiredTransitive(AU);
    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
}

bool CheckContractPass::runOnFunction(llvm::Function& F) {

    DM = &GetAnalysis<DefectManager>::doit(this, F);
    FM = &GetAnalysis<FunctionManager>::doit(this, F);
    MI = &GetAnalysis<MetaInfoTrackerPass>::doit(this, F);
    PSA = &GetAnalysis<PredicateStateAnalysis>::doit(this, F);

    auto* slotTracker = GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(F);
    PF = PredicateFactory::get(slotTracker);
    TF = TermFactory::get(slotTracker);

    PSF = PredicateStateFactory::get();

    CallInstVisitor civ(this);
    civ.visit(F);

    return false;
}

CheckContractPass::~CheckContractPass() {}

char CheckContractPass::ID;
static RegisterPass<CheckContractPass>
X("check-contracts", "Pass that checks annotated code contracts");

} /* namespace borealis */