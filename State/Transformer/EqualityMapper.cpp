/*
 * EqualityMapper.cpp
 *
 *  Created on: 25 мая 2015 г.
 *      Author: kivi
 */

#include "State/Transformer/EqualityMapper.h"
#include "Util/option.hpp"

namespace borealis {

EqualityMapper::EqualityMapper() : Base(FactoryNest()) {}

Predicate::Ptr EqualityMapper::transformEqualityPredicate(EqualityPredicatePtr pred) {
    if(util::at(mapping, pred->getLhv())) return pred;
    if(auto&& optRef = util::at(mapping, pred->getRhv())) {
        auto&& re = optRef.get();
        mapping[pred->getLhv()] = *re;
    } else mapping[pred->getLhv()] = pred->getRhv();
    return pred;
}

std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals> EqualityMapper::getMappedValues() const {
    return mapping;
}

}  /* namespace borealis */


