/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <State/Transformer/Unifier.h>
#include <State/Transformer/StateOptimizer.h>
#include "ContractManager.h"
#include "State/Transformer/StateChoiceKiller.h"
#include "State/Transformer/StateMergingTransformer.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module&) {
    return false;
}

void ContractManager::addContract(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping) {
    ++calls[F];
    if (not S->isEmpty()) {
        auto&& optimized = optimizeState(S, FN);
        auto&& choiceKilled = killStateChoice(optimized, FN);
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
            auto&& unified = unifyState(choiceKilled, FN, F, argumentsReplacement);
            states[F].insert(unified);
        }
    }
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {
    auto&& dbg = dbgs();

    dbg << "Contract extraction results" << endl;

    for (auto&& it : states) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;
        dbg << "Called " << calls[it.first] << " times" << endl;
        dbg << endl;

        dbg << "Arguments:" << endl;
        dbg << arguments[it.first] << endl;
        dbg << endl;

        auto&& merger = StateMergingTransformer(FactoryNest());
        for (auto&& state : it.second) {
            merger.transform(state);
            dbg << "State:" << endl;
            dbg << state << endl;
        }

        dbg << endl;
        /*auto&& predicates = merger.getPredicates();

        dbg << "Predinditions:" << endl;
        dbg << "(" << endl;
        for (auto&& it : predicates) {
            dbg << "  " << it.first << " : " << it.second << endl;
        }
        dbg << ")" << endl << endl;*/
    }

    dbg << end;
}

PredicateState::Ptr ContractManager::optimizeState(PredicateState::Ptr S, const FactoryNest& FN) {
    auto&& optimizer = StateOptimizer(FN);
    return optimizer.transform(S);
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
std::unordered_map<llvm::Function*, int> ContractManager::calls;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
