//
// Created by kivi on 30.06.15.
//

#ifndef STATE_TRANSFORMER_STATECHOISEKILLER_H
#define STATE_TRANSFORMER_STATECHOISEKILLER_H

#include "Transformer.hpp"

namespace borealis {

class StateChoiceKiller : public Transformer<StateChoiceKiller> {

    using Base = Transformer<StateChoiceKiller>;

public:

    StateChoiceKiller(const FactoryNest& fn);

    using Base::transform;
    PredicateState::Ptr transform(PredicateState::Ptr ps);
    PredicateState::Ptr transformPredicateStateChoice(PredicateStateChoicePtr ps);

    bool isChanged();

private:

    FactoryNest FN;
    bool changed;

};

}  /* namespace borealis */

#endif //STATE_TRANSFORMER_STATECHOISEKILLER_H
