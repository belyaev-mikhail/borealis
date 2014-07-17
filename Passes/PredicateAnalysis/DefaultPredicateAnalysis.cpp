/*
 * DefaultPredicateAnalysis.cpp
 *
 *  Created on: Aug 31, 2012
 *      Author: ice-phoenix
 */

#include <llvm/Support/InstVisitor.h>

#include <vector>

#include "Codegen/llvm.h"
#include "Passes/PredicateAnalysis/DefaultPredicateAnalysis.h"
#include "Passes/Tracker/SlotTrackerPass.h"

#include "Util/macros.h"

namespace borealis {

using borealis::util::tail;
using borealis::util::view;

////////////////////////////////////////////////////////////////////////////////
//
// Default predicate analysis instruction visitor
//
////////////////////////////////////////////////////////////////////////////////

class DPAInstVisitor : public llvm::InstVisitor<DPAInstVisitor> {

public:

    DPAInstVisitor(DefaultPredicateAnalysis* pass) : pass(pass) {}

    void visitLoadInst(llvm::LoadInst& I) {
        using llvm::Value;

        Value* lhv = &I;
        Value* rhv = I.getPointerOperand();
        
        pass->PM[&I] = pass->FN.Predicate->getLoadPredicate(
            getValueTermWithTypes(lhv),
            pass->FN.Term->getLoadTerm(
                getValueTermWithTypes(rhv)
            ),
            pass->SLT->getLocFor(&I)
        );
    }

    void visitStoreInst(llvm::StoreInst& I) {
        using llvm::Value;

        Value* lhv = I.getPointerOperand();
        Value* rhv = I.getValueOperand();

        pass->PM[&I] = pass->FN.Predicate->getStorePredicate(
            getValueTermWithTypes(lhv),
            getValueTermWithTypes(rhv),
            pass->SLT->getLocFor(&I)
        );
    }

    void visitCmpInst(llvm::CmpInst& I) {
        using llvm::ConditionType;
        using llvm::Value;

        Value* lhv = &I;
        Value* op1 = I.getOperand(0);
        Value* op2 = I.getOperand(1);
        ConditionType cond = conditionType(I.getPredicate());

        pass->PM[&I] = pass->FN.Predicate->getEqualityPredicate(
            getValueTermWithTypes(lhv),
            pass->FN.Term->getCmpTerm(
                cond,
                getValueTermWithTypes(op1),
                getValueTermWithTypes(op2)
            ),
            pass->SLT->getLocFor(&I)
        );
    }

    void visitBranchInst(llvm::BranchInst& I) {
        using llvm::BasicBlock;

        if (I.isUnconditional()) return;

        Term::Ptr condTerm = getValueTermWithTypes(I.getCondition());
        const BasicBlock* trueSucc = I.getSuccessor(0);
        const BasicBlock* falseSucc = I.getSuccessor(1);

        pass->TPM[{&I, trueSucc}] =
            pass->FN.Predicate->getBooleanPredicate(
                condTerm,
                pass->FN.Term->getTrueTerm(),
                pass->SLT->getLocFor(&I)
            );
        pass->TPM[{&I, falseSucc}] =
            pass->FN.Predicate->getBooleanPredicate(
                condTerm,
                pass->FN.Term->getFalseTerm(),
                pass->SLT->getLocFor(&I)
            );
    }

    void visitSwitchInst(llvm::SwitchInst& I) {
        using llvm::BasicBlock;

        Term::Ptr condTerm = getValueTermWithTypes(I.getCondition());

        std::vector<Term::Ptr> cases;
        cases.reserve(I.getNumCases());

        for (auto c = I.case_begin(); c != I.case_end(); ++c) {
            Term::Ptr caseTerm = pass->FN.Term->getConstTerm(c.getCaseValue());
            const BasicBlock* caseSucc = c.getCaseSuccessor();

            pass->TPM[{&I, caseSucc}] =
                pass->FN.Predicate->getEqualityPredicate(
                    condTerm,
                    caseTerm,
                    pass->SLT->getLocFor(&I),
                    PredicateType::PATH
                );

            cases.push_back(caseTerm);
        }

        const BasicBlock* defaultSucc = I.getDefaultDest();
        pass->TPM[{&I, defaultSucc}] =
            pass->FN.Predicate->getDefaultSwitchCasePredicate(
                condTerm,
                cases,
                pass->SLT->getLocFor(&I)
            );
    }

    void visitSelectInst(llvm::SelectInst& I) {
        using llvm::Value;

        Value* lhv = &I;
        Value* cnd = I.getCondition();
        Value* tru = I.getTrueValue();
        Value* fls = I.getFalseValue();

        pass->PM[&I] = pass->FN.Predicate->getEqualityPredicate(
            getValueTermWithTypes(lhv),
            pass->FN.Term->getTernaryTerm(
                getValueTermWithTypes(cnd),
                getValueTermWithTypes(tru),
                getValueTermWithTypes(fls)
            ),
            pass->SLT->getLocFor(&I)
        );
    }

    void visitGetElementPtrInst(llvm::GetElementPtrInst& I) {
        using namespace llvm;

        Value* lhv = &I;
        Value* rhv = I.getPointerOperand();

        std::vector<llvm::Value*> idxs;
        idxs.reserve(I.getNumOperands() - 1);
        for (auto& i : tail(view(I.op_begin(), I.op_end()))) {
            idxs.push_back(i);
        }

        pass->PM[&I] = pass->FN.Predicate->getEqualityPredicate(
            getValueTermWithTypes(lhv),
            pass->FN.Term->getGepTerm(rhv, idxs),
            pass->SLT->getLocFor(&I)
        );
    }

    void visitCastInst(llvm::CastInst& I) {
        using namespace llvm;

        Value* lhv = &I;
        Value* rhv = I.getOperand(0);

        auto cast = I.getOpcode();

        Term::Ptr lhvt;
        Term::Ptr rhvt;
        if (Instruction::CastOps::FPToSI ==  cast) {
            lhvt = getValueTermWithTypes(lhv, Signedness::Signed);
        } else {
            lhvt = getValueTermWithTypes(lhv, Signedness::Unsigned);
        }

        if (Instruction::CastOps::SIToFP ==  cast) {
            rhvt = getValueTermWithTypes(rhv, Signedness::Signed);
        } else {
            rhvt = getValueTermWithTypes(rhv, Signedness::Unsigned);
        }

        if (Instruction::CastOps::FPExt == cast) {
            pass->PM[&I] = pass->FN.Predicate->getEqualityPredicate(
                lhvt,
                rhvt,
                pass->SLT->getLocFor(&I)
            );
            return;
        }

        pass->PM[&I] = pass->FN.Predicate->getCastPredicate(
            lhvt,
            rhvt,
            pass->SLT->getLocFor(&I)
        );
    }

    void visitPHINode(llvm::PHINode& I) {
        using namespace llvm;

        for (unsigned int i = 0; i < I.getNumIncomingValues(); i++) {
            const BasicBlock* from = I.getIncomingBlock(i);
            Value* v = I.getIncomingValue(i);

            pass->PPM[{from, &I}] = pass->FN.Predicate->getEqualityPredicate(
                getValueTermWithTypes(&I),
                getValueTermWithTypes(v),
                pass->SLT->getLocFor(&I)
            );
        }
    }

    void visitBinaryOperator(llvm::BinaryOperator& I) {
        using namespace llvm;

        typedef llvm::Instruction::BinaryOps OPS;

        Value* lhv = &I;
        Value* op1 = I.getOperand(0);
        Value* op2 = I.getOperand(1);
        ArithType type = arithType(I.getOpcode());

        pass->PM[&I] = pass->FN.Predicate->getEqualityPredicate(
            getValueTermWithTypes(lhv),
            pass->FN.Term->getBinaryTerm(
                type,
                getValueTermWithTypes(op1),
                getValueTermWithTypes(op2)
            ),
            pass->SLT->getLocFor(&I)
        );
    }

    void visitReturnInst(llvm::ReturnInst& I) {
        using llvm::Value;

        Value* rv = I.getReturnValue();
        if (rv == nullptr) return;

        pass->PM[&I] = pass->FN.Predicate->getEqualityPredicate(
            pass->FN.Term->getReturnValueTerm(I.getParent()->getParent()),
            getValueTermWithTypes(rv),
            pass->SLT->getLocFor(&I)
        );
    }

private:

    Term::Ptr getValueTermWithTypes(llvm::Value* v, llvm::Signedness sign = llvm::Signedness::Unknown) {
        auto vds = pass->MIT->locate(v);
        
        auto types = util::viewContainer(vds)
                .map([](decltype(*vds.begin()) vd){return vd.type;})
                .toVector();
                
        return pass->FN.Term->getValueTerm(v, sign, types);
    }

    
    DefaultPredicateAnalysis* pass;

};

////////////////////////////////////////////////////////////////////////////////

DefaultPredicateAnalysis::DefaultPredicateAnalysis() :
        ProxyFunctionPass(ID) {}

DefaultPredicateAnalysis::DefaultPredicateAnalysis(llvm::Pass* pass) :
        ProxyFunctionPass(ID, pass) {}

void DefaultPredicateAnalysis::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();

    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
    AUX<SourceLocationTracker>::addRequiredTransitive(AU);
    AUX<llvm::TargetData>::addRequiredTransitive(AU);
    AUX<MetaInfoTracker>::addRequiredTransitive(AU);
}

bool DefaultPredicateAnalysis::runOnFunction(llvm::Function& F) {
    init();

    auto* st = GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(F);
    FN = FactoryNest(st);

    SLT = &GetAnalysis<SourceLocationTracker>::doit(this, F);
    TD = &GetAnalysis<llvm::TargetData>::doit(this, F);
    MIT = &GetAnalysis<MetaInfoTracker>::doit(this, F);

    DPAInstVisitor visitor(this);
    visitor.visit(F);

    return false;
}

DefaultPredicateAnalysis::~DefaultPredicateAnalysis() {}

////////////////////////////////////////////////////////////////////////////////

char DefaultPredicateAnalysis::ID;
static RegisterPass<DefaultPredicateAnalysis>
X("default-predicate-analysis", "Default instruction predicate analysis");

} /* namespace borealis */

#include "Util/unmacros.h"
