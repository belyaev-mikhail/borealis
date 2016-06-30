/*
 * UnusedVariableDeleter.cpp
 *
 *  Created on: May 30, 2016
 *      Author: stepanov
 */

#include "State/Transformer/UnusedVariablesDeleter.h"
#include "State/Transformer/TermCollector.h"

#include "Util/util.h"

#include "Util/macros.h"

namespace borealis {

UnusedVariablesDeleter::UnusedVariablesDeleter(FactoryNest FN, const TermSet& ts) :
        Base(FN), usedVars(ts) {}

static struct {
    using argument_type = Term::Ptr;

    bool operator()(Term::Ptr t) const {
        return llvm::isa<ArgumentTerm>(t) ||
               llvm::isa<ReturnValueTerm>(t) ||
               llvm::isa<ValueTerm>(t);
    }
} isInterestingTerm;

PredicateState::Ptr UnusedVariablesDeleter::transform(PredicateState::Ptr ps) {
           return Base::transform(ps->reverse())
                   ->filter([](auto&& p) { return !!p; })
                   ->reverse()
                   ->simplify();
}

Predicate::Ptr UnusedVariablesDeleter::transformBase(Predicate::Ptr pred) {

    if(llvm::is_one_of<EqualityPredicate, AllocaPredicate, MallocPredicate>(pred) &&
            PredicateType::STATE == pred->getType()){
        auto&& lhvTerms = Term::Set{};
        for (auto&& lhv : util::viewContainer(pred->getOperands()).take(1)) {
            auto&& nested = Term::getFullTermSet(lhv);
            util::viewContainer(nested)
                    .filter(isInterestingTerm)
                    .foreach(APPLY(lhvTerms.insert));
        }
        if(util::hasIntersect(usedVars, lhvTerms)){
            for (auto&& rhv : util::viewContainer(pred->getOperands()).drop(1)) {
                auto&& nested = Term::getFullTermSet(rhv);
                util::viewContainer(nested)
                        .filter(isInterestingTerm)
                        .foreach(APPLY(usedVars.insert));
            }
            return pred;
        }
        return nullptr;
    }
    else {
        for (auto&& rhv : util::viewContainer(pred->getOperands())) {
            auto&& nested = Term::getFullTermSet(rhv);
            util::viewContainer(nested)
                    .filter(isInterestingTerm)
                    .foreach(APPLY(usedVars.insert));
        }
    }
    return pred;
}

#include "Util/unmacros.h"

} /* namespace borealis */