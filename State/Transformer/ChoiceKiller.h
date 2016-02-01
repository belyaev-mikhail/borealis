//
// Created by kivi on 30.06.15.
//

#ifndef STATE_TRANSFORMER_STATECHOISEKILLER_H
#define STATE_TRANSFORMER_STATECHOISEKILLER_H

#include "Transformer.hpp"

namespace borealis {

class ChoiceKiller : public Transformer<ChoiceKiller> {

    using Base = Transformer<ChoiceKiller>;
    using MemInfo = std::pair<unsigned int, unsigned int>;

public:

    using States = std::vector<PredicateState::Ptr>;

    ChoiceKiller(const FactoryNest& fn, MemInfo f);

    using Base::transform;
    PredicateState::Ptr transform(PredicateState::Ptr ps);
    PredicateState::Ptr transformPredicateStateChoice(PredicateStateChoicePtr ps);

private:

    FactoryNest FN;
    MemInfo fMemInfo;

    bool containsState(const States& states, const PredicateState::Ptr value);

};

}  /* namespace borealis */

#endif //STATE_TRANSFORMER_STATECHOISEKILLER_H
