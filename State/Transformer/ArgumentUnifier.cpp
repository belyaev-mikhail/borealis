//
// Created by kivi on 06.07.15.
//

#include "ArgumentUnifier.h"

namespace borealis {

ArgumentUnifier::ArgumentUnifier(const FactoryNest& fn, const ArgToTerms& a)
        : Base(fn), FN(fn), argsMapping(a) {
}

PredicateState::Ptr ArgumentUnifier::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();
}

Predicate::Ptr ArgumentUnifier::transformPredicate(Predicate::Ptr pred) {
    for (auto&& op : pred->getOperands()) {
        if (not allowedInContract(op)) {
            return nullptr;
        }
    }
    return pred;
}

Term::Ptr ArgumentUnifier::transformTerm(Term::Ptr term) {
    for (auto&& it : argsMapping) {
        if (util::contains(it.second, term)) {
            auto&& type = term->getType();
            auto&& newTerm = FN.Term->getArgumentTermExternal(it.first, ARGUMENT_PREFIX + std::to_string(it.first), type);
            args.insert(newTerm);
            return newTerm;
        }
    }
    return term;
}

const ArgumentUnifier::TermSet& ArgumentUnifier::getArguments() {
    return args;
}

bool ArgumentUnifier::allowedInContract(Term::Ptr term) const {
    for (auto&& t : term->getSubterms()) {
        auto&& valTerm = llvm::dyn_cast<ValueTerm>(t);
        auto isGlobal = valTerm && valTerm->isGlobal();
        if (not isOpaqueTerm(t) && not llvm::isa<ArgumentTerm>(t) && not isGlobal) {
            return false;
        }
    }
    return true;
}

bool ArgumentUnifier::isOpaqueTerm(Term::Ptr term) const {
    return llvm::is_one_of<
            OpaqueBoolConstantTerm,
            OpaqueIntConstantTerm,
            OpaqueFloatingConstantTerm,
            OpaqueStringConstantTerm,
            OpaqueNullPtrTerm
    >(term);
}

}   /* namespace borealis */