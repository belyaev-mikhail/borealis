//
// Created by kivi on 30.06.15.
//

#include <unordered_map>

#include "SMT/Z3/Z3.h"
#include "SMT/Z3/ExprFactory.h"
#include "SMT/Z3/Solver.h"

#include "ChoiceKiller.h"

namespace borealis {

ChoiceKiller::ChoiceKiller(const FactoryNest& fn, MemInfo f) : Base(fn), FN(fn), fMemInfo(f) {}

PredicateState::Ptr ChoiceKiller::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();
}

PredicateState::Ptr ChoiceKiller::transformPredicateStateChoice(PredicateStateChoicePtr ps) {
    States choices;
    for (auto&& state : ps->getChoices()) {
        if (not containsState(choices, state)) {
            choices.push_back(state);
        }
    }

    std::sort(choices.begin(), choices.end(), [](auto&& a, auto&& b) -> bool {
       return a->size() < b->size();
    });

    auto&& newChoice = FN.State->Choice(choices);

    Z3::ExprFactory ef;
    Z3::Solver s(ef, fMemInfo.first, fMemInfo.second);
    auto res = s.isFullGroup(newChoice);

    if (res.isSat()) {
        return newChoice;
    } else {
        return FN.State->Basic();
    }
}

bool ChoiceKiller::containsState(const States& states, const PredicateState::Ptr value) {
    for (auto&& state : states) {
        if (state->equals(value.get())) {
            return true;
        }
    }
    return false;
}

}  /* namespace borealis */