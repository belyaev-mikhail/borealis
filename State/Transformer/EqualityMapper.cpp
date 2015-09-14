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

    auto&& newRhv = Term::Ptr{ pred->getRhv()->replaceOperands(boolTermsMapping) };
    auto&& newPredicate = Predicate::Ptr{ pred->replaceOperands({ {pred->getRhv(), newRhv} }) };
    auto&& newEqualityPredicate = llvm::dyn_cast<EqualityPredicate>(newPredicate.get());

    if (auto&& value = util::at(mapping, newEqualityPredicate->getRhv())) {
        mapping[newEqualityPredicate->getLhv()] = value.getUnsafe();
    } else {
        mapping[newEqualityPredicate->getLhv()] = newEqualityPredicate->getRhv();
    }

    if (llvm::is_one_of<BinaryTerm, CmpTerm>(pred->getRhv())) {
        if (auto&& value = util::at(boolTermsMapping, newEqualityPredicate->getRhv())) {
            boolTermsMapping[newEqualityPredicate->getLhv()] = value.getUnsafe();
        } else {
            boolTermsMapping[newEqualityPredicate->getLhv()] = newEqualityPredicate->getRhv();
        }
    }

    return newEqualityPredicate->shared_from_this();
}

Predicate::Ptr EqualityMapper::transformPredicate(Predicate::Ptr pred) {
    return pred;
}

const EqualityMapper::TermMap& EqualityMapper::getMappedValues() const {
    return mapping;
}

}  /* namespace borealis */
