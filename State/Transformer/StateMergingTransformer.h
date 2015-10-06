//
// Created by kivi on 07.07.15.
//

#ifndef BOREALIS_STATEMERGINGTRANSFORMER_H
#define BOREALIS_STATEMERGINGTRANSFORMER_H

#include "Transformer.hpp"

namespace borealis {

class StateMergingTransformer : public Transformer<StateMergingTransformer> {

    using Base = Transformer<StateMergingTransformer>;
    using PredicatesCounter = std::unordered_map<Predicate::Ptr, int, PredicateHash, PredicateEquals>;

public:

    StateMergingTransformer(const FactoryNest& fn);

    using Base::transform;
    PredicateState::Ptr transform(PredicateState::Ptr ps);
    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    const PredicatesCounter& getPredicates();

private:

    FactoryNest FN;
    PredicatesCounter predicates;

};

}   /* namespace borealis */

#endif //BOREALIS_STATEMERGINGTRANSFORMER_H
