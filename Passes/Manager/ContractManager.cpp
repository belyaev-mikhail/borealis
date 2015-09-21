/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <State/Transformer/Unifier.h>
#include <State/Transformer/StateOptimizer.h>
#include <State/Transformer/StateMergingTransformer.h>
#include "ContractManager.h"
#include "State/Transformer/StateChoiceKiller.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module&) {
    return false;
}

void ContractManager::addContract(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping) {
    ++calls[F];
    if (not S->isEmpty()) {
        auto&& optimized = StateOptimizer(FN).transform(S);
        PredicateState::Ptr choiceKilled;
        while (true) {
            auto&& killer = StateChoiceKiller(FN);
            choiceKilled = killer.transform(optimized);
            if (not killer.isChanged()) {
                break;
            }
            optimized = StateOptimizer(FN).transform(choiceKilled);
        }
        if(not choiceKilled->isEmpty()) {
            for (auto&& it : mapping) {
                if (not util::containsKey(arguments[F], it.first)) {
                   arguments[F][it.first] = *it.second.begin();
                }
                for (auto&& term : it.second) {
                    argsReplacement[F][term] = arguments[F][it.first];
                }
            }
            auto&& unified = Unifier(FN, arguments[F], argsReplacement[F]).transform(choiceKilled);
            states[F].insert(unified);
        }
    }
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {
    auto&& dbg = dbgs();

    dbg << endl << "Contract extraction results" << endl;

    for (auto&& it : states) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;
        dbg << "Called " << calls[it.first] << " times" << endl;
        dbg << endl;

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

char ContractManager::ID = 0;
ContractManager::ContractStates ContractManager::states;
ContractManager::ContractArguments ContractManager::arguments;
std::unordered_map<llvm::Function*, int> ContractManager::calls;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
