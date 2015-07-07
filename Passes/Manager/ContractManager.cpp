/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <State/Transformer/Unifier.h>
#include "ContractManager.h"
#include "State/Transformer/StateChoiceKiller.h"
#include "State/Transformer/StateMergingTransformer.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module&) {
    return false;
}

void ContractManager::addContract(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping) {
    if (not S->isEmpty()) {
        auto&& choiceKilled = killStateChoice(S, FN);
        if(not choiceKilled->isEmpty()) {
            TermMap argumentsReplacement;
            for (auto&& it : mapping) {
                if (not util::containsKey(arguments[F], it.first)) {
                   arguments[F][it.first] = *it.second.begin();
                }
                for (auto&& term : it.second) {
                    argumentsReplacement[term] = arguments[F][it.first];
                }
            }
            auto&& unifiedState = unifyState(choiceKilled, FN, F, argumentsReplacement);
            states[F].insert(unifiedState);
        }
    }
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {
    auto&& dbg = dbgs();

    dbg << "Contract extraction results" << endl;

    for (auto&& it : states) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;

        dbg << "Arguments:" << endl;
        dbg << arguments[it.first] << endl;
        dbg << endl;

        for (auto&& state : it.second) {
            dbg << "State:" << endl;
            dbg << state << endl;
        }
        dbg << endl;
    }

    dbg << end;
}

PredicateState::Ptr ContractManager::killStateChoice(PredicateState::Ptr S, const FactoryNest& FN) {
    auto&& choiceKiller = StateChoiceKiller(FN);
    return choiceKiller.transform(S);
}

PredicateState::Ptr ContractManager::unifyState(PredicateState::Ptr S, const FactoryNest& FN, llvm::Function* F, const TermMap& argumentsReplacement) {
    auto&& unifier = Unifier(FN, arguments[F], argumentsReplacement);
    return unifier.transform(S);
}

PredicateState::Ptr ContractManager::mergeState(PredicateState::Ptr S, const FactoryNest& FN) {
    auto&& merger = StateMergingTransformer(FN);
    return merger.transform(S);
}

char ContractManager::ID = 0;
ContractManager::ContractStates ContractManager::states;
ContractManager::ContractArguments ContractManager::arguments;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
