/*
 * EqualityMapper.cpp
 *
 *  Created on: 25 мая 2015 г.
 *      Author: kivi
 */

#include "EqualityMapper.h"

namespace borealis {

EqualityMapper::EqualityMapper(FactoryNest FN) : Base(FN) {}

Predicate::Ptr EqualityMapper::transformEqualityPredicate(EqualityPredicatePtr pred) {
    if (util::at(mapping, pred->getLhv())) return pred;

    TermMap replacement;
    for (auto&& subterm : Term::getFullTermSet(pred->getRhv())) {
        if (auto&& value = util::at(mapping, subterm)) {
            replacement[subterm] = value.getUnsafe();
        }
    }

    auto&& newRhv = Term::Ptr{ pred->getRhv()->replaceOperands(replacement) };
    auto&& replaced = FN.Predicate->getEqualityPredicate(pred->getLhv(), newRhv, pred->getLocation(), pred->getType());

    if (auto&& newEq = llvm::dyn_cast<EqualityPredicate>(replaced)) {
        mapping[newEq->getLhv()] = newEq->getRhv();
    }

    return replaced;
}

Predicate::Ptr EqualityMapper::transformPredicate(Predicate::Ptr pred) {
    if (pred->getType() == PredicateType::PATH) {
        return Predicate::Ptr{ pred->replaceOperands(mapping) };
    }
    return pred;
}

const EqualityMapper::TermMap& EqualityMapper::getMappedValues() const {
    return mapping;
}

}  /* namespace borealis */
