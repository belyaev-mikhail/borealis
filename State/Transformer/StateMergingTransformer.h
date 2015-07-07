//
// Created by kivi on 07.07.15.
//

#ifndef BOREALIS_STATEMERGINGTRANSFORMER_H
#define BOREALIS_STATEMERGINGTRANSFORMER_H

#include "Transformer.hpp"

namespace borealis {

class StateMergingTransformer : public Transformer<StateMergingTransformer> {

    using Base = Transformer<StateMergingTransformer>;

public:

    StateMergingTransformer(const FactoryNest& fn);

    PredicateState::Ptr transform(PredicateState::Ptr ps);
    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

private:

    FactoryNest FN;
};

}   /* namespace borealis */

#endif //BOREALIS_STATEMERGINGTRANSFORMER_H
