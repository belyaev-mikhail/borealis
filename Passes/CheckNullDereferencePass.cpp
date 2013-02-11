/*
 * CheckNullDereferencePass.cpp
 *
 *  Created on: Aug 24, 2012
 *      Author: ice-phoenix
 */

#include <llvm/BasicBlock.h>
#include <llvm/Constants.h>
#include <llvm/Support/InstVisitor.h>

#include "lib/poolalloc/src/DSA/DataStructureAA.h"

#include "Passes/CheckNullDereferencePass.h"
#include "Passes/DefaultPredicateAnalysis.h"
#include "Query/AndQuery.h"
#include "Query/EqualityQuery.h"
#include "Query/NullPtrQuery.h"
#include "Solver/Z3Solver.h"

#include "Logging/logger.hpp"

namespace borealis {

class DerefInstVisitor : public llvm::InstVisitor<DerefInstVisitor> {

public:

    DerefInstVisitor(CheckNullDereferencePass* pass) : pass(pass) {}

    void visitLoadInst(llvm::LoadInst& I) {
        using llvm::AliasAnalysis;
        using llvm::Value;

        Value* ptr = I.getPointerOperand();
        if (ptr->isDereferenceablePointer()) return;

        for (auto* derefNullValue : *(pass->DerefNullSet)) {
            if (pass->AA->alias(ptr, derefNullValue) != AliasAnalysis::AliasResult::NoAlias) {
                pass->ValueNullSet->insert(&I);
                break;
            }
        }
    }

private:

    CheckNullDereferencePass* pass;

};

class ValueInstVisitor :
    public llvm::InstVisitor<ValueInstVisitor> {

public:

    ValueInstVisitor(CheckNullDereferencePass* pass) : pass(pass) {}

    void visitLoadInst(llvm::LoadInst& I) {
        using llvm::AliasAnalysis;
        using llvm::Value;

        Value* ptr = I.getPointerOperand();
        if (ptr->isDereferenceablePointer()) return;

        for (auto* nullValue : *(pass->ValueNullSet)) {
            if (pass->AA->alias(ptr, nullValue) != AliasAnalysis::AliasResult::NoAlias) {
                if (checkNullDereference(I, *ptr, *nullValue)) {
                    reportNullDereference(I, *ptr, *nullValue);
                    break;
                }
            }
        }
    }

    void visitStoreInst(llvm::StoreInst& I) {
        using llvm::AliasAnalysis;
        using llvm::Value;

        Value* ptr = I.getPointerOperand();
        if (ptr->isDereferenceablePointer()) return;

        for (auto* nullValue : *(pass->ValueNullSet)) {
            if (pass->AA->alias(ptr, nullValue) != AliasAnalysis::AliasResult::NoAlias) {
                if (checkNullDereference(I, *ptr, *nullValue)) {
                    reportNullDereference(I, *ptr, *nullValue);
                    break;
                }
            }
        }
    }

    bool checkNullDereference(
            llvm::Instruction& where,
            llvm::Value& what,
            llvm::Value& why) {

        using llvm::dyn_cast;
        using llvm::Instruction;

        NullPtrQuery q = NullPtrQuery(&what, pass->slotTracker);

        pass->infos() << "Query: " << q.toString() << endl;

        PredicateStateVector psv = pass->PSA->getPredicateStateMap()[&where];
        for (const auto& ps : psv) {

            pass->infos() << "Checking state: " << ps << endl;

            if (!ps.hasVisited({
                    &where,
                    dyn_cast<const Instruction>(&what),
                    dyn_cast<const Instruction>(&why)
            })) {

                pass->infos() << "Infeasible!" << endl;

                continue;
            }

            z3::context ctx;
            Z3ExprFactory z3ef(ctx);
            Z3Solver s(z3ef);

            if (s.checkSatOrUnknown(q, ps)) {
                pass->infos() << "SAT" << endl;
                return true;
            }
        }

        pass->infos() << "UNSAT" << endl;
        return false;
    }

    void reportNullDereference(
            llvm::Instruction& where,
            llvm::Value& /* what */,
            llvm::Value& /* from */) {
        pass->defectManager->addDefect(DefectType::INI_03, &where);
    }

private:

    CheckNullDereferencePass* pass;

};

CheckNullDereferencePass::CheckNullDereferencePass() : ProxyFunctionPass() {}
CheckNullDereferencePass::CheckNullDereferencePass(llvm::Pass* pass) : ProxyFunctionPass(pass) {}

void CheckNullDereferencePass::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
    using namespace::llvm;

    Info.setPreservesAll();
    Info.addRequiredTransitive<AliasAnalysis>();

    Info.addRequiredTransitive<DefaultPredicateAnalysis::PSA::MX>();
    Info.addRequiredTransitive<DetectNullPass::MX>();

    Info.addRequiredTransitive<DefectManager>();
    Info.addRequiredTransitive<SlotTrackerPass>();
}

bool CheckNullDereferencePass::runOnFunction(llvm::Function& F) {
    using namespace::llvm;

    AA = &getAnalysis<AliasAnalysis>();

    PSA = &getAnalysis<DefaultPredicateAnalysis::PSA::MX>().getResultsForFunction(&F);
    DNP = &getAnalysis<DetectNullPass::MX>().getResultsForFunction(&F);

    defectManager = &getAnalysis<DefectManager>();
    slotTracker = getAnalysis<SlotTrackerPass>().getSlotTracker(F);

    auto valueSet = DNP->getNullSet(NullType::VALUE);
    auto derefSet = DNP->getNullSet(NullType::DEREF);

    ValueNullSet = &valueSet;
    DerefNullSet = &derefSet;

    DerefInstVisitor div(this);
    div.visit(F);

    ValueInstVisitor viv(this);
    viv.visit(F);

    return false;
}

void CheckNullDereferencePass::print(llvm::raw_ostream& s, const llvm::Module*) const {
    s << "DerefNullSet:" << endl << *DerefNullSet << endl;
    s << "ValueNullSet:" << endl << *ValueNullSet << endl;
}

CheckNullDereferencePass::~CheckNullDereferencePass() {}

char CheckNullDereferencePass::ID;
static RegisterModularizedPass<CheckNullDereferencePass>
X("check-null-deref", "NULL dereference checker");

} /* namespace borealis */
