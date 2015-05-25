/*
 * EqualityMapping.h
 *
 *  Created on: 25 мая 2015 г.
 *      Author: kivi
 */

#ifndef STATE_TRANSFORMER_EQUALITYMAPPER_H_
#define STATE_TRANSFORMER_EQUALITYMAPPER_H_

#include "State/Transformer/Transformer.hpp"
#include <unordered_map>

namespace borealis {

class EqualityMapper: public borealis::Transformer<EqualityMapper> {

    using Base = borealis::Transformer<EqualityMapper>;

public:

    EqualityMapper();

    Predicate::Ptr transformEqualityPredicate(EqualityPredicatePtr pred);


private:
    std::unordered_map<Term::Ptr, Term::Ptr> mapping;
};

}  /* namespace borealis */



#endif /* STATE_TRANSFORMER_EQUALITYMAPPER_H_ */
