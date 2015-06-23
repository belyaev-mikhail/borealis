/*
 * EqualityMapper.cpp
 *
 *  Created on: 25 мая 2015 г.
 *      Author: kivi
 */

#include "EqualityMapper.h"
#include "../../Util/collections.hpp"

namespace borealis {

EqualityMapper::EqualityMapper(FactoryNest FN) : Base(FN) {}

Predicate::Ptr EqualityMapper::transformEqualityPredicate(EqualityPredicatePtr pred) {
    if (util::at(mapping, pred->getLhv())) return pred;

    if (auto&& optRef = util::at(mapping, pred->getRhv())) {
        mapping[pred->getLhv()] = optRef.getUnsafe();
    } else {
        mapping[pred->getLhv()] = pred->getRhv();
    }

    return pred;
}

const EqualityMapper::TermMap& EqualityMapper::getMappedValues() const {
    return mapping;
}

}  /* namespace borealis */
