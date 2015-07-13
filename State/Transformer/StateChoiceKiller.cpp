//
// Created by kivi on 30.06.15.
//

#include <unordered_map>

#include "StateChoiceKiller.h"

namespace borealis {

StateChoiceKiller::StateChoiceKiller(const FactoryNest& fn) : Base(fn) {
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
    std::unordered_map<Predicate::Ptr, PredicateState::Ptr, PredicateHash, PredicateEquals> statesMap;

    auto minSize = UINT32_MAX;
    for (auto&& it : ps->getChoices()) {
        if (auto&& basicState = llvm::dyn_cast<BasicPredicateState>(it)) {
            if (not basicState->isEmpty()) {
                basicStates.push_back(const_cast<BasicPredicateState*>(basicState));
                if (basicState->size() < minSize) {
                    minSize = basicState->size();
                }
            }
        } else if (not it->isEmpty()){
            newChoice.push_back(it);
        }
    }

    auto equalPredicates = 0U;
    if (not basicStates.empty()) {
        for(; equalPredicates < minSize; ++equalPredicates) {
            bool isBreak = false;
            for (auto i = 1U; i < basicStates.size(); ++i) {
                auto&& data = basicStates[i]->getData();
                if (not data[equalPredicates]->equals(basicStates[0]->getData()[equalPredicates].get())) {
                    isBreak = true;
                    break;
                }
            }
            if (isBreak) {
                break;
            }
        }
        if (equalPredicates >= minSize) {
            equalPredicates = minSize - 1;
        }

        for (auto&& it : basicStates) {
            statesMap[it->getData()[equalPredicates]] = it->shared_from_this();
        }
    }


    std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals> boolInv;
    boolInv[FN.Term->getTrueTerm()] = FN.Term->getFalseTerm();
    boolInv[FN.Term->getFalseTerm()] = FN.Term->getTrueTerm();

    for (auto&& it : statesMap) {
        auto&& inverted = Predicate::Ptr{ it.first->replaceOperands(boolInv) };
        if (auto&& optRef = util::at(statesMap, inverted)) {
            auto&& state = optRef.getUnsafe();
            auto counter = 0U;
            newChoice.push_back(state->filter([&](auto&&) { return (counter++ < equalPredicates) ? true : false; })
                                        ->simplify());
        } else {
            newChoice.push_back(it.second);
        }
    }

    return FN.State->Choice(newChoice);
}

}  /* namespace borealis */