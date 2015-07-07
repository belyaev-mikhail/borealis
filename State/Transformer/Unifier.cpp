//
// Created by kivi on 06.07.15.
//

#include "Unifier.h"

namespace borealis {

Unifier::Unifier(const FactoryNest& fn, const std::unordered_map<int, Term::Ptr>& a, const TermMap& m) : Base(fn), FN(fn), termMap(m) {
    for (auto&& it : a) {
        args.insert(it.second);
    }
}

Predicate::Ptr Unifier::transformEqualityPredicate(EqualityPredicatePtr pred) {
    if (auto&& cmp = llvm::dyn_cast<CmpTerm>(pred->getLhv())) {
        auto cond = invertCondition(cmp->getOpcode());
        if (isInverted) {
            auto&& term = FN.Term->getCmpTerm(cond, cmp->getLhv(), cmp->getRhv());
            auto&& boolean = invertBoolean(pred->getRhv());
            return FN.Predicate->getEqualityPredicate(term, boolean, pred->getLocation(), pred->getType());
        }
    }
    return pred;
}

Term::Ptr Unifier::transformTerm(Term::Ptr term) {
    if (auto&& optRef = util::at(termMap, term)) {
        return optRef.getUnsafe();
    }
    return term;
}

Term::Ptr Unifier::transformCmpTerm(CmpTermPtr term) {
    switch (term->getOpcode()) {
        case llvm::ConditionType::GT:
            if (auto&& op = llvm::dyn_cast<OpaqueIntConstantTerm>(term->getRhv())) {
                return FN.Term->getCmpTerm(llvm::ConditionType::GE, term->getLhv(), FN.Term->getOpaqueConstantTerm(op->getValue() + 1));
            } break;
        default:
            break;
    }
    return term;
}

llvm::ConditionType Unifier::invertCondition(llvm::ConditionType cond) {
    switch (cond) {
        case llvm::ConditionType::LT:
            isInverted = true;
            return llvm::ConditionType::GE;
        case llvm::ConditionType::LE:
            isInverted = true;
            return llvm::ConditionType::GT;
        case llvm::ConditionType::ULT:
            isInverted = true;
            return llvm::ConditionType::UGE;
        case llvm::ConditionType::ULE:
            isInverted = true;
            return llvm::ConditionType::UGT;
        case llvm::ConditionType::EQ:
            isInverted = true;
            return llvm::ConditionType::NEQ;
        default:
            isInverted = false;
            return cond;
    }
}

Term::Ptr Unifier::invertBoolean(Term::Ptr term) {
    if (term->equals(FN.Term->getTrueTerm().get()))
        return FN.Term->getFalseTerm();
    else return FN.Term->getTrueTerm();
}

}   /* namespace borealis */