//
// Created by kivi on 06.07.15.
//

#include "Unifier.h"

namespace borealis {

Unifier::Unifier(const FactoryNest& fn, const std::unordered_map<int, Term::Ptr>& a, const TermMap& m)
        : Base(fn), FN(fn), argsReplacement(m) {
    for (auto&& it : a) {
        args.insert(it.second);
    }
}

Predicate::Ptr Unifier::transformEqualityPredicate(EqualityPredicatePtr pred) {
    auto&& reverted = llvm::dyn_cast<EqualityPredicate>(revertEqualityPredicate(pred));
    if (auto&& cmp = llvm::dyn_cast<CmpTerm>(reverted->getLhv())) {
        auto cond = invertCondition(cmp->getOpcode());
        if (isInverted) {
            auto&& term = FN.Term->getCmpTerm(cond, cmp->getLhv(), cmp->getRhv());
            auto&& boolean = invertBoolean(reverted->getRhv());
            return FN.Predicate->getEqualityPredicate(term, boolean, reverted->getLocation(), reverted->getType());
        }
    }
    return reverted->shared_from_this();
}

Term::Ptr Unifier::transformTerm(Term::Ptr term) {
    if (auto&& value = util::at(argsReplacement, term)) {
        return value.getUnsafe();
    }
    return term;
}

Term::Ptr Unifier::transformCmpTerm(CmpTermPtr term) {
    auto&& revertedTerm = revertCmpTerm(term);
    const CmpTerm* reverted = llvm::dyn_cast<CmpTerm>(revertedTerm);
    return reverted->shared_from_this();
}

Term::Ptr Unifier::transformBinaryTerm(BinaryTermPtr term) {
    if (containArgs(term->getRhv()) && not containArgs(term->getLhv())) {
        return FN.Term->getBinaryTerm(term->getOpcode(), term->getRhv(), term->getLhv());
    }
    if (llvm::isa<OpaqueBoolConstantTerm>(term->getLhv())) {
        return FN.Term->getBinaryTerm(term->getOpcode(), term->getRhv(), term->getLhv());
    }
    return term;
}


llvm::ConditionType Unifier::invertCondition(llvm::ConditionType cond) {
    isInverted = true;
    switch (cond) {
        case llvm::ConditionType::LT:
            return llvm::ConditionType::GE;
        case llvm::ConditionType::LE:
            return llvm::ConditionType::GT;
        case llvm::ConditionType::ULT:
            return llvm::ConditionType::UGE;
        case llvm::ConditionType::ULE:
            return llvm::ConditionType::UGT;
        case llvm::ConditionType::EQ:
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

Term::Ptr Unifier::revertCmpTerm(CmpTermPtr term) {
    auto&& createRevertedCmpTerm = [&] (llvm::ConditionType cond) -> Term::Ptr {
        return FN.Term->getCmpTerm(cond, term->getRhv(), term->getLhv());
    };
    if (util::contains(args, term->getRhv()) && not util::contains(args, term->getLhv())) {
        switch (term->getOpcode()) {
            case llvm::ConditionType::GT:
                return createRevertedCmpTerm(llvm::ConditionType::LT);
            case llvm::ConditionType::LT:
                return createRevertedCmpTerm(llvm::ConditionType::GT);
            case llvm::ConditionType::GE:
                return createRevertedCmpTerm(llvm::ConditionType::LE);
            case llvm::ConditionType::LE:
                return createRevertedCmpTerm(llvm::ConditionType::GT);
            case llvm::ConditionType::UGT:
                return createRevertedCmpTerm(llvm::ConditionType::ULT);
            case llvm::ConditionType::ULT:
                return createRevertedCmpTerm(llvm::ConditionType::UGT);
            case llvm::ConditionType::UGE:
                return createRevertedCmpTerm(llvm::ConditionType::ULE);
            case llvm::ConditionType::ULE:
                return createRevertedCmpTerm(llvm::ConditionType::UGT);
            case llvm::ConditionType::NEQ:
                return createRevertedCmpTerm(llvm::ConditionType::NEQ);
            case llvm::ConditionType::EQ:
                return createRevertedCmpTerm(llvm::ConditionType::EQ);
            default:
                return term;
        }
    }
    return term;
}

Predicate::Ptr Unifier::revertEqualityPredicate(EqualityPredicatePtr pred) {
    if (containArgs(pred->getRhv()) && not containArgs(pred->getLhv())) {
        return FN.Predicate->getEqualityPredicate(pred->getRhv(), pred->getLhv(), pred->getLocation(), pred->getType());
    }
    return pred;
}

bool Unifier::containArgs(Term::Ptr term) {
    for (auto&& subterm : Term::getFullTermSet(term)) {
        if (util::contains(args, subterm)) {
            return true;
        }
    }
    return false;
}

}   /* namespace borealis */