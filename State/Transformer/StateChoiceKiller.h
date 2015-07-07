//
// Created by kivi on 30.06.15.
//

#ifndef STATE_TRANSFORMER_STATECHOISEKILLER_H
#define STATE_TRANSFORMER_STATECHOISEKILLER_H

#include "Transformer.hpp"

namespace borealis {

struct PredicateHash {
    size_t operator()(Predicate::Ptr pred) const noexcept {
        return pred->hashCode();
    }
};

struct PredicateEquals {
    bool operator()(Predicate::Ptr lhv, Predicate::Ptr rhv) const noexcept {
        return lhv->equals(rhv.get());
    }
};


class StateChoiceKiller : public Transformer<StateChoiceKiller> {

    using Base = Transformer<StateChoiceKiller>;

public:

    StateChoiceKiller(const FactoryNest& fn);

    PredicateState::Ptr transform(PredicateState::Ptr ps);
    PredicateState::Ptr transformPredicateStateChoice(PredicateStateChoicePtr ps);

private:

    FactoryNest FN;
};

}  /* namespace borealis */

#endif //STATE_TRANSFORMER_STATECHOISEKILLER_H
