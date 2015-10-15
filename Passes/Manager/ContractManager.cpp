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

void ContractManager::addContract(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S,
                                  const std::unordered_map<int, Args>& mapping) {
    ++calls[F];
    if (not S->isEmpty()) {
        for (auto&& it : mapping) {
            if (not util::containsKey(contractArguments[F], it.first)) {
                auto&& type = (*it.second.begin())->getType();
                auto&& arg = FN.Term->getValueTerm(type,"arg%" + std::to_string(it.first));
                contractArguments[F][it.first] = arg;
            }
            for (auto&& term : it.second) {
                contractArgsReplacement[F][term] = contractArguments[F][it.first];
            }
        }
        auto&& unified = Unifier(FN, contractArguments[F], contractArgsReplacement[F]).transform(S);
        auto&& optimized = StateOptimizer(FN).transform(unified);
        auto&& choiceKilled = optimized;
        while (true) {
            auto&& killer = StateChoiceKiller(FN);
            choiceKilled = killer.transform(optimized);
            if (not killer.isChanged()) {
                break;
            }
            optimized = StateOptimizer(FN).transform(choiceKilled);
        }
        if (not choiceKilled->isEmpty()) {
            contracts[F].insert(choiceKilled);
        }
    }
}

void ContractManager::addSummary(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S,
                                  const std::unordered_map<int, Args>& mapping) {
    if (not S->isEmpty()) {
        for (auto&& it : mapping) {
            if (not util::containsKey(summaryArguments[F], it.first)) {
                auto&& type = (*it.second.begin())->getType();
                auto&& arg = FN.Term->getValueTerm(type,"arg%" + std::to_string(it.first));
                summaryArguments[F][it.first] = arg;
            }
            for (auto&& term : it.second) {
                summaryArgsReplacement[F][term] = summaryArguments[F][it.first];
            }
        }
        auto&& unified = Unifier(FN, summaryArguments[F], summaryArgsReplacement[F]).transform(S);
        auto&& optimized = StateOptimizer(FN).transform(unified);
        if (not optimized->isEmpty()) {
            summaries[F].insert(optimized);
        }
    }
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {
    auto&& dbg = dbgs();

    dbg << "Contract extraction results" << endl;

    for (auto&& it : contracts) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;
        dbg << "Called " << calls[it.first] << " times" << endl;
        dbg << endl;

        auto&& merger = StateMergingTransformer(FactoryNest());
        for (auto&& state : it.second) {
            dbg << "State:" << endl;
            dbg << state << endl;
            merger.transform(state);
        }
        //dbg << "Merged:" << endl;
        //dbg << merger.getPredicates() << endl << endl;

        dbg << endl;
    }

    /*dbg << "Summary extraction results" << endl;

    for (auto&& it : summaries) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;
        dbg << "Called " << calls[it.first] << " times" << endl;
        dbg << endl;

        for (auto&& state : it.second) {
            dbg << "State:" << endl;
            dbg << state << endl;
        }

        dbg << endl;
    }*/

    dbg << end;
}

char ContractManager::ID = 0;
ContractManager::ContractStates ContractManager::contracts;
ContractManager::ContractArguments ContractManager::contractArguments;
std::unordered_map<llvm::Function*, int> ContractManager::calls;

ContractManager::ContractStates ContractManager::summaries;
ContractManager::ContractArguments ContractManager::summaryArguments;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
