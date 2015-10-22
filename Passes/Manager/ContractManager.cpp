/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <State/Transformer/Unifier.h>
#include <State/Transformer/StateOptimizer.h>
#include <State/Transformer/MergingTransformer.h>

#include "ContractManager.h"
#include "State/Transformer/StateChoiceKiller.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module&) {
    return false;
}

void ContractManager::addContract(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S,
                                  const std::unordered_map<int, Args>& mapping) {
    ++functionCalls[F];
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
            saveState(F, choiceKilled);
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
    printContracts();
    printSummaries();
}

void ContractManager::saveState(llvm::Function* F, PredicateState::Ptr S) {
    if (auto&& choice = llvm::dyn_cast<PredicateStateChoice>(S)) {
        choiceContracts[F].insert(choice->shared_from_this());
    } else if (auto&& basic = llvm::dyn_cast<BasicPredicateState>(S)) {
        basicContracts[F].insert(basic->shared_from_this());
    } else {
        auto&& chain = llvm::cast<PredicateStateChain>(S);
        saveState(F, chain->getBase());
        saveState(F, chain->getCurr());
    }
}

void ContractManager::getUniqueChoices(llvm::Function* F, std::vector<std::pair<PredicateState::Ptr, int>>& res) const {
    for (auto&& state : choiceContracts[F]) {
        bool added = false;
        for (auto&& it_choice : res) {
            if (it_choice.first->equals(state.get())) {
                ++it_choice.second;
                added = true;
                break;
            }
        }
        if (not added) {
            res.push_back({state, 1});
        }
    }
}

void ContractManager::printContracts() const {
    ContractStates result;

    //analyzing results
    for (auto&& it : functionCalls) {
        auto&& F = it.first;
        auto&& calls = it.second;
        //merging basic states for each function, probability 0.66
        auto&& merger = MergingTransformer(FactoryNest(nullptr), calls);
        for (auto&& state : basicContracts[F]) {
            merger.transform(state);
        }
        auto&& merged = merger.getMergedState(0.66);
        if (not merged->isEmpty()) {
            result[F].insert(merged);
        }
        //merging choice states for each function, probability 0.66
        if (calls > 1) {
            std::vector<std::pair<PredicateState::Ptr, int>> choices;
            getUniqueChoices(F, choices);
            for (auto&& it_choice : choices) {
                if (double(it_choice.second)/double(calls) > 0.66) {
                    result[F].insert(it_choice.first);
                }
            }
        }
    }

    //printing results
    auto&& dbg = dbgs();

    dbg << "Contract extraction results" << endl;
    for (auto&& it : result) {
        auto&& F = it.first;
        dbg << "---" << "Function " << F->getName() << "---" << endl;

        for(auto&& state : it.second) {
            dbg << "State:" << endl;
            dbg << state << endl;
        }

        dbg << endl;
    }
    dbg << end;
}

void ContractManager::printSummaries() const {
    auto&& dbg = dbgs();

    dbg << "Summary extraction results" << endl;
    for (auto&& it : summaries) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;

        for (auto&& state : it.second) {
            dbg << "State:" << endl;
            dbg << state << endl;
        }

        dbg << endl;
    }
    dbg << end;
}

char ContractManager::ID = 0;
ContractManager::ContractStates ContractManager::choiceContracts;
ContractManager::ContractStates ContractManager::basicContracts;
ContractManager::ContractArguments ContractManager::contractArguments;
std::unordered_map<llvm::Function*, int> ContractManager::functionCalls;

ContractManager::ContractStates ContractManager::summaries;
ContractManager::ContractArguments ContractManager::summaryArguments;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
