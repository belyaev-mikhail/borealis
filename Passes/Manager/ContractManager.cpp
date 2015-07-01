/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include "ContractManager.h"
#include "../../Logging/logger.hpp"
#include "../../Util/passes.hpp"
#include "../../Util/collections.hpp"
#include "../../State/Transformer/TermReplaceTransformer.h"
#include "../../State/Transformer/StateChoiceKiller.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module&) {
    return false;
}

void ContractManager::addContract(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping) {
    if (not S->isEmpty()) {
        TermMap argumentsReplacement;
        for (auto&& it : mapping) {
            if (not util::containsKey(arguments[F], it.first)) {
               arguments[F][it.first] = *it.second.begin();
            }
            for (auto&& term : it.second) {
                argumentsReplacement[term] = arguments[F][it.first];
            }
        }
        auto&& killed = StateChoiceKiller(FN).transform(S);
        if(not killed->isEmpty()) {
            auto&& state = replaceTerms(killed, FN, argumentsReplacement);
            states[F].insert(state);
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

PredicateState::Ptr ContractManager::replaceTerms(PredicateState::Ptr S, const FactoryNest& FN, const TermMap& argumentsReplacement) {
    auto&& termReplace = TermReplaceTransformer(FN, argumentsReplacement);
    return termReplace.transform(S);
}

char ContractManager::ID = 0;
ContractManager::ContractStates ContractManager::states;
ContractManager::ContractArguments ContractManager::arguments;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
