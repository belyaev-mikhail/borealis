//
// Created by kivi on 30.06.15.
//

#include <unordered_map>

#include "SMT/Z3/Z3.h"
#include "SMT/Z3/ExprFactory.h"
#include "SMT/Z3/Solver.h"

#include "ChoiceOptimizer.h"

namespace borealis {

ChoiceOptimizer::ChoiceOptimizer(const FactoryNest& fn) : Base(fn), FN(fn) {}

PredicateState::Ptr ChoiceOptimizer::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();
}

PredicateState::Ptr ChoiceOptimizer::transformPredicateStateChoice(PredicateStateChoicePtr ps) {
    States choices;
    for (auto&& state : ps->getChoices()) {
        if (not containsState(choices, state)) {
            choices.push_back(state);
        }
    }

    std::sort(choices.begin(), choices.end(), [](auto&& a, auto&& b) -> bool {
       return a->size() < b->size();
    });

    return FN.State->Choice(choices);
}

bool ChoiceOptimizer::containsState(const States& states, const PredicateState::Ptr value) {
    for (auto&& state : states) {
        if (state->equals(value.get())) {
            return true;
        }
    }
    return false;
}

}  /* namespace borealis */