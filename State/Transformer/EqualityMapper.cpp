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
    if(auto&& optRef = util::at(mapping, pred->getRhv())) {
    	auto&& re=optRef.get();
        mapping[pred->getLhv()] = *re;
    } else mapping[pred->getLhv()] = pred->getRhv();
    return pred;
}

}  /* namespace borealis */


