//
// Created by kivi on 30.06.15.
//

#include <unordered_map>

#include "StateChoiceKiller.h"

namespace borealis {

StateChoiceKiller::StateChoiceKiller(const FactoryNest& fn) : Base(fn), changed(false) {
    FN = fn;
}

PredicateState::Ptr StateChoiceKiller::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();
}

PredicateState::Ptr StateChoiceKiller::transformPredicateStateChoice(PredicateStateChoicePtr ps) {
    std::vector<PredicateState::Ptr> newChoice;
    std::vector<BasicPredicateState*> basicStates;
    PredicatesMap statesMap;

    auto minSize = UINT32_MAX;
    for (auto&& choice : ps->getChoices()) {
        if (auto&& basicState = llvm::dyn_cast<BasicPredicateState>(choice)) {
            if (not basicState->isEmpty()) {
                basicStates.push_back(const_cast<BasicPredicateState*>(basicState));
                if (basicState->size() < minSize) minSize = basicState->size();
            }
        } else if (not choice->isEmpty()){
            newChoice.push_back(choice);
        }
    }

    auto equalPredicates = 0U;

    if (basicStates.size() == 1) {
        newChoice.push_back(basicStates[0]->shared_from_this());
        return FN.State->Choice(newChoice);
    } else if (not basicStates.empty()) {
        for (equalPredicates = 0U; equalPredicates < minSize; ++equalPredicates) {
            if (not isAllPredicatesEqual(equalPredicates, basicStates)) {
                break;
            }
        }
        for (auto&& it : basicStates) {
            if (it->size() <= equalPredicates) {
                newChoice.push_back(it->shared_from_this());
            } else {
                statesMap[it->getData()[equalPredicates]] = it->shared_from_this();
            }
        }
    }

    std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals> boolInv;
    boolInv[FN.Term->getTrueTerm()] = FN.Term->getFalseTerm();
    boolInv[FN.Term->getFalseTerm()] = FN.Term->getTrueTerm();

    for (auto&& it : statesMap) {
        auto&& inverted = Predicate::Ptr{ it.first->replaceOperands(boolInv) };
        if (auto&& value = util::at(statesMap, inverted)) {
            changed = true;
            auto&& state = value.getUnsafe();
            auto counter = 0U;
            newChoice.push_back(state->filter([&](auto&&) { return (counter++ < equalPredicates) ? true : false; })
                                        ->simplify());
        } else {
            newChoice.push_back(it.second);
        }
    }

    auto&& unique = getUniqueStates(newChoice);
    if (unique.size() == 1) {
        if (auto&& basic = llvm::dyn_cast<BasicPredicateState>(unique[0])) {
            changed = true;
            return FN.State->Basic(basic->getData());
        }
    }

    return FN.State->Choice(unique);
}

bool StateChoiceKiller::isChanged() {
    return changed;
}

std::vector<PredicateState::Ptr> StateChoiceKiller::getUniqueStates(std::vector<PredicateState::Ptr>& states) {
    std::vector<PredicateState::Ptr> unique;
    for (auto&& state : states) {
        bool isUnique = true;
        for (auto && it : unique) {
            if (it->equals(state.get())) {
                isUnique = false;
                break;
            }
        }
        if (isUnique) {
            unique.push_back(state);
        }
    }
    return unique;
}

bool StateChoiceKiller::isAllPredicatesEqual(const int predicateIndex, std::vector<BasicPredicateState*>& states) {
    auto&& firstData = states[0]->getData();
    for (auto i = 1U; i < states.size(); ++i) {
        auto&& data = states[i]->getData();
        if (not data[predicateIndex]->equals(firstData[predicateIndex].get())) {
            return false;
        }
    }
    return true;
}

}  /* namespace borealis */