/*
 * DefaultPredicateAnalysis.cpp
 *
 *  Created on: Aug 31, 2012
 *      Author: ice-phoenix
 */

#include <llvm/Support/InstVisitor.h>

#include <vector>

#include "Logging/tracer.hpp"
#include "Passes/DefaultPredicateAnalysis.h"
#include "Passes/SlotTrackerPass.h"

#include "Util/macros.h"

namespace borealis {

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

        pass->PM[&I] = pass->PF->getLoadPredicate(
                pass->TF->getValueTerm(lhv),
                pass->TF->getValueTerm(rhv)
        );
    }

    void visitStoreInst(llvm::StoreInst& I) {
        using llvm::Value;

        Value* lhv = I.getPointerOperand();
        Value* rhv = I.getValueOperand();

        pass->PM[&I] = pass->PF->getStorePredicate(
                pass->TF->getValueTerm(lhv),
                pass->TF->getValueTerm(rhv)
        );
    }

    void visitICmpInst(llvm::ICmpInst& I) {
        using llvm::Value;

        Value* lhv = &I;
        Value* op1 = I.getOperand(0);
        Value* op2 = I.getOperand(1);
        llvm::ConditionType pred = conditionType(I.getPredicate());

        pass->PM[&I] = pass->PF->getICmpPredicate(
                pass->TF->getValueTerm(lhv),
                pass->TF->getValueTerm(op1),
                pass->TF->getValueTerm(op2),
                pred
        );
    }

    void visitBranchInst(llvm::BranchInst& I) {
        using llvm::BasicBlock;
        using llvm::Value;

        if (I.isUnconditional()) return;

        Value* cond = I.getCondition();
        const BasicBlock* trueSucc = I.getSuccessor(0);
        const BasicBlock* falseSucc = I.getSuccessor(1);

        pass->TPM[{&I, trueSucc}] =
                pass->PF->getBooleanPredicate(
                        pass->TF->getValueTerm(cond),
                        pass->TF->getTrueTerm()
                );
        pass->TPM[{&I, falseSucc}] =
                pass->PF->getBooleanPredicate(
                        pass->TF->getValueTerm(cond),
                        pass->TF->getFalseTerm()
                );
    }

    void visitSwitchInst(llvm::SwitchInst& I) {
        using llvm::BasicBlock;

        Term::Ptr condTerm = pass->TF->getValueTerm(I.getCondition());
        std::vector<Term::Ptr> cases;
        cases.reserve(I.getNumCases());
        for (auto c = I.case_begin(); c != I.case_end(); ++c) {
            Term::Ptr caseTerm = pass->TF->getConstTerm(c.getCaseValue());
            BasicBlock* caseSucc = c.getCaseSuccessor();

            pass->TPM[{&I, caseSucc}] =
                    pass->PF->getEqualityPredicate(
                            condTerm,
                            caseTerm
                    );

            cases.push_back(caseTerm);
        }

        BasicBlock* defaultSucc = I.getDefaultDest();
        pass->TPM[{&I, defaultSucc}] =
                pass->PF->getDefaultSwitchCasePredicate(
                        condTerm,
                        cases
                );
    }

    void visitAllocaInst(llvm::AllocaInst& I) {
        using llvm::BasicBlock;
        using llvm::Value;

        Value* lhv = &I;
        Value* numElems = I.getArraySize();

        pass->PM[&I] = pass->PF->getAllocaPredicate(
                pass->TF->getValueTerm(lhv),
                pass->TF->getValueTerm(numElems)
        );
    }

    void visitGetElementPtrInst(llvm::GetElementPtrInst& I) {
        using namespace llvm;

        llvm::Type* type = I.getPointerOperandType();

        std::vector< std::pair<Term::Ptr, Term::Ptr> > shifts;
        shifts.reserve(I.getNumIndices());
        for (auto it = I.idx_begin(); it != I.idx_end(); ++it) {
            Value* v = *it;
            Term::Ptr by = pass->TF->getValueTerm(v);
            Term::Ptr size = pass->TF->getIntTerm(
                    pass->TD->getTypeAllocSize(type));
            shifts.push_back({by, size});

            if (type->isArrayTy()) type = type->getArrayElementType();
            else if (type->isStructTy()) {
                if (!isa<ConstantInt>(v)) {
                    BYE_BYE_VOID("Non-constant structure field shift");
                }
                auto fieldIdx = cast<ConstantInt>(v)->getZExtValue();
                type = type->getStructElementType(fieldIdx);
            }
        }

        Value* lhv = &I;
        Value* rhv = I.getPointerOperand();

        pass->PM[&I] = pass->PF->getGEPPredicate(
                pass->TF->getValueTerm(lhv),
                pass->TF->getValueTerm(rhv),
                shifts
        );
    }

    void visitSExtInst(llvm::SExtInst& I) {
        using llvm::Value;

        Value* lhv = &I;
        Value* rhv = I.getOperand(0);

        pass->PM[&I] = pass->PF->getEqualityPredicate(
                pass->TF->getValueTerm(lhv),
                pass->TF->getValueTerm(rhv)
        );
    }

    void visitBitCastInst(llvm::BitCastInst& I) {
        using llvm::Value;

        Value* lhv = &I;
        Value* rhv = I.getOperand(0);

        pass->PM[&I] = pass->PF->getEqualityPredicate(
                pass->TF->getValueTerm(lhv),
                pass->TF->getValueTerm(rhv)
        );
    }

    void visitPHINode(llvm::PHINode& I) {
        using llvm::BasicBlock;
        using llvm::PHINode;
        using llvm::Value;

        for (unsigned int i = 0; i < I.getNumIncomingValues(); i++) {
            const BasicBlock* from = I.getIncomingBlock(i);
            Value* v = I.getIncomingValue(i);

            pass->PPM[{from, &I}] = pass->PF->getEqualityPredicate(
                    pass->TF->getValueTerm(&I),
                    pass->TF->getValueTerm(v)
            );
        }
    }

    void visitBinaryOperator(llvm::BinaryOperator& I) {
        using llvm::Value;
        using llvm::ArithType;

        typedef llvm::Instruction::BinaryOps OPS;

        Value* lhv = &I;
        Value* op1 = I.getOperand(0);
        Value* op2 = I.getOperand(1);
        OPS opCode = I.getOpcode();

        ArithType type;

        switch (opCode) {
        case OPS::Add:
        case OPS::FAdd:
            type = ArithType::ADD;
            break;
        case OPS::Sub:
        case OPS::FSub:
            type = ArithType::SUB;
            break;
        case OPS::Mul:
        case OPS::FMul:
            type = ArithType::MUL;
            break;
        case OPS::UDiv:
        case OPS::SDiv:
        case OPS::FDiv:
            type = ArithType::DIV;
            break;
        default:
            return; // FIXME: Do smth?
        }

        pass->PM[&I] = pass->PF->getArithPredicate(
                pass->TF->getValueTerm(lhv),
                pass->TF->getValueTerm(op1),
                pass->TF->getValueTerm(op2),
                type
        );
    }

private:

    DefaultPredicateAnalysis* pass;

};

////////////////////////////////////////////////////////////////////////////////

DefaultPredicateAnalysis::DefaultPredicateAnalysis() :
        ProxyFunctionPass(ID) {}

DefaultPredicateAnalysis::DefaultPredicateAnalysis(llvm::Pass* pass) :
        ProxyFunctionPass(ID, pass) {}

void DefaultPredicateAnalysis::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    using namespace::llvm;

    AU.setPreservesAll();

    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
    AUX<TargetData>::addRequiredTransitive(AU);
}

bool DefaultPredicateAnalysis::runOnFunction(llvm::Function& F) {
    using namespace::llvm;

    TRACE_FUNC;

    init();

    auto* ST = GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(F);

    PF = PredicateFactory::get(ST);
    TF = TermFactory::get(ST);
    TD = &GetAnalysis<TargetData>::doit(this, F);

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
