//
// Created by kivi on 06.07.15.
//

#include "Unifier.h"

namespace borealis {

Unifier::Unifier(const FactoryNest& fn, const ArgToTerms& a)
        : Base(fn), FN(fn), argsMapping(a) {
}

Predicate::Ptr Unifier::transformEqualityPredicate(EqualityPredicatePtr pred) {
    auto&& reverted = llvm::dyn_cast<EqualityPredicate>(revertEqualityPredicate(pred));
    if (auto&& cmp = llvm::dyn_cast<CmpTerm>(reverted->getLhv())) {
        auto cond = invertCondition(cmp->getOpcode());
        if (cond != cmp->getOpcode()) {
            auto&& term = FN.Term->getCmpTerm(cond, cmp->getLhv(), cmp->getRhv());
            auto&& boolean = invertBoolean(reverted->getRhv());
            return FN.Predicate->getEqualityPredicate(term, boolean, reverted->getLocation(), reverted->getType());
        }
    }
    return reverted->shared_from_this();
}

Term::Ptr Unifier::transformTerm(Term::Ptr term) {
    for (auto&& it : argsMapping) {
        if (util::contains(it.second, term)) {
            auto&& type = term->getType();
            auto&& newTerm = FN.Term->getValueTerm(type, ARGUMENT_PREFIX + std::to_string(it.first));
            args.insert(newTerm);
            return newTerm;
        }
    }
    return term;
}

Term::Ptr Unifier::transformCmpTerm(CmpTermPtr term) {
    auto&& reverted = revertCmpTerm(term);
    auto&& revCmp = llvm::cast<CmpTerm>(reverted);
    if (isSigned(revCmp->getOpcode())) {
        TermMap replacement;
        for (auto&& subterm : Term::getFullTermSet(revCmp->shared_from_this())) {
            if (auto&& intTerm = llvm::dyn_cast<OpaqueIntConstantTerm>(subterm)) {
                if (auto&& type = llvm::dyn_cast<borealis::type::Integer>(subterm->getType())) {
                    long long maxVal = 1;
                    maxVal <<= type->getBitsize();
                    if (intTerm->getValue() > ((maxVal / 2) - 1)) {
                        --maxVal;
                        replacement[subterm] = FN.Term->getIntTerm(-(maxVal - intTerm->getValue() + 1),
                                                                   term->getType());
                    }
                }
            }
        }
        return Term::Ptr{ revCmp->replaceOperands(replacement) };
    }
    return reverted;
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

bool Unifier::isSigned(llvm::ConditionType cond) {
    using CondType = llvm::ConditionType;
    if (cond != CondType::ULT && cond != CondType::ULE && cond != CondType::UGE && cond != CondType::UGT)
        return true;
    return false;
}

const Unifier::TermSet& Unifier::getArguments() {
    return args;
}

}   /* namespace borealis */