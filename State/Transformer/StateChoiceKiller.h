//
// Created by kivi on 30.06.15.
//

#ifndef STATE_TRANSFORMER_STATECHOISEKILLER_H
#define STATE_TRANSFORMER_STATECHOISEKILLER_H

#include "Transformer.hpp"

namespace borealis {

class StateChoiceKiller : public Transformer<StateChoiceKiller> {

    using Base = Transformer<StateChoiceKiller>;
    using PredicatesMap = std::unordered_map<Predicate::Ptr, PredicateState::Ptr, PredicateHash, PredicateEquals>;

public:

    StateChoiceKiller(const FactoryNest& fn);

    PredicateState::Ptr transform(PredicateState::Ptr ps);
    PredicateState::Ptr transformPredicateStateChoice(PredicateStateChoicePtr ps);

    bool isChanged();

private:

    FactoryNest FN;
    bool changed;

    bool isAllStatesEqual(std::vector<PredicateState::Ptr>& states);
    bool isAllPredicatesEqual(const int predicateIndex, std::vector<BasicPredicateState*>& states);

};

}  /* namespace borealis */

#endif //STATE_TRANSFORMER_STATECHOISEKILLER_H
