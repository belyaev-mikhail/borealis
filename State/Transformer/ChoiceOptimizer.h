//
// Created by kivi on 30.06.15.
//

#ifndef STATE_TRANSFORMER_STATECHOISEKILLER_H
#define STATE_TRANSFORMER_STATECHOISEKILLER_H

#include "Transformer.hpp"

namespace borealis {

class ChoiceOptimizer : public Transformer<ChoiceOptimizer> {

    using Base = Transformer<ChoiceOptimizer>;

public:

    using States = std::vector<PredicateState::Ptr>;

    ChoiceOptimizer(const FactoryNest& fn);

    using Base::transform;
    PredicateState::Ptr transform(PredicateState::Ptr ps);
    PredicateState::Ptr transformPredicateStateChoice(PredicateStateChoicePtr ps);

private:

    FactoryNest FN;

    bool containsState(const States& states, const PredicateState::Ptr value);

};

}  /* namespace borealis */

#endif //STATE_TRANSFORMER_STATECHOISEKILLER_H
