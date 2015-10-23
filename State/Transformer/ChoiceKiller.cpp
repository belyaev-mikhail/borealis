//
// Created by kivi on 30.06.15.
//

#include <unordered_map>

#include "ChoiceKiller.h"

namespace borealis {

ChoiceKiller::ChoiceKiller(const FactoryNest& fn) : Base(fn), FN(fn), changed(false) {}

PredicateState::Ptr ChoiceKiller::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();
}

PredicateState::Ptr ChoiceKiller::transformPredicateStateChoice(PredicateStateChoicePtr ps) {
    std::unordered_map<int, States> choices;
    States newChoice;
    for (auto&& state : ps->getChoices()) {
        if (not containsState(choices[state->size()], state)) {
            choices[state->size()].push_back(state);
        }
    }

    for (auto&& choice : choices) {
        States removed;
        removeFullGroups(choice.second, removed);
        for (auto&& state : removed) {
            newChoice.push_back(state);
        }
    }

    return FN.State->Choice(newChoice);
}

bool ChoiceKiller::isChanged() {
    return changed;
}

void ChoiceKiller::removeFullGroups(const States& states, States& removed) {
    auto stateSize = states[0]->size();
    auto numOfCombinations = unsigned(std::pow(2, stateSize));
    if (states.size() < numOfCombinations) {
        removed = states;
        return;
    } else {
        std::vector<States> groups;
        getDifferentGroups(states, groups);
        for (auto&& group : groups) {
            if (group.size() < numOfCombinations) {
                for (auto&& state : group) removed.push_back(state);
            } else {
                changed = true;
            }
        }
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

bool ChoiceKiller::isConditionsEqual(PredicateState::Ptr a, PredicateState::Ptr b) {
    auto&& first = llvm::dyn_cast<BasicPredicateState>(a);
    auto&& second = llvm::dyn_cast<BasicPredicateState>(b);
    if (first != nullptr && second != nullptr && first->size() == second->size()) {
        auto predicateIndex = 0U;
        for(; predicateIndex < first->size(); ++predicateIndex) {
            auto&& firstCond = llvm::dyn_cast<EqualityPredicate>(first->getData()[predicateIndex]);
            auto&& secondCond = llvm::dyn_cast<EqualityPredicate>(second->getData()[predicateIndex]);
            if (firstCond != nullptr && secondCond != nullptr) {
                if (not firstCond->getLhv()->equals(secondCond->getLhv().get())) {
                    break;
                }
            }
        }
        return predicateIndex == first->size();
    }
    return false;
}


void ChoiceKiller::getDifferentGroups(const States& states, std::vector<States>& groups) {
    std::vector<bool> visited(states.size(), false);
    for (auto i = 0U; i < states.size(); ++i) {
        if (not visited[i]) {
            States group;
            visited[i] = true;
            group.push_back(states[i]);
            for (auto j = i + 1; j < states.size(); ++j) {
                if (not visited[j] && isConditionsEqual(states[i], states[j])) {
                    visited[j] = true;
                    group.push_back(states[j]);
                }
            }
            groups.push_back(group);
        }
    }
}

}  /* namespace borealis */