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
    std::unordered_map<Predicate::Ptr, PredicateState::Ptr, PredicateHash, PredicateEquals> statesMap;
    for (auto&& it : ps->getChoices()) {
        if (auto&& basicState = llvm::dyn_cast<BasicPredicateState>(it)) {
            statesMap[*basicState->getData().begin()] = it;
        } else {
            newChoice.push_back(it);
        }
    }

    std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals> boolInv;
    boolInv[FN.Term->getTrueTerm()] = FN.Term->getFalseTerm();
    boolInv[FN.Term->getFalseTerm()] = FN.Term->getTrueTerm();

    for (auto&& it : statesMap) {
        auto&& inverted = Predicate::Ptr{ it.first->replaceOperands(boolInv) };
        if (auto&& optRef = util::at(statesMap, inverted)) {
            auto&& state = optRef.getUnsafe();
            newChoice.push_back(state->filter([](auto&&) { return nullptr; })
                                        ->simplify());
        }
    }
    return FN.State->Choice(newChoice);
}

}  /* namespace borealis */