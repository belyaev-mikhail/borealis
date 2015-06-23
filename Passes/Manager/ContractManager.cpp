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

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module&) {
    replaseTerms();
    return false;
}

void ContractManager::addContract(llvm::Function* F, PredicateState::Ptr S, const ArgToTerms& mapping) {
    if (not S->isEmpty()) {
        states[F].insert(S);
        for (auto&& it : mapping) {
            for (auto&& terms : it.second) {
                arguments[F][it.first].insert(terms);
            }
        }
    }
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {
    auto&& dbg = dbgs();

    for (auto&& it : states) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;

        dbg << "Arguments:" << endl;
        dbg << arguments[it.first] << endl;
        for (auto&& state : it.second) {
            dbg << "State:" << endl;
            dbg << state << endl;
        }
        dbg << endl;
    }

    dbg << end;
}

void ContractManager::replaseTerms() {
    /*for (auto&& it : states) {
        std::unordered_map<int, Term::Ptr> argMap;
        std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals> termMap;

        for (auto&& stateInfo : it.second) {
            for (auto&& map_it : stateInfo.mapping) {
                if (not util::containsKey(argMap, map_it.first)) {
                    argMap[map_it.first] = *map_it.second.begin();
                }

                for (auto&& term : map_it.second) {
                    termMap[term] = argMap[map_it.first];
                }
            }
        }

        ArgToTerms args;
        for(auto&& arg : argMap) {
            args[arg.first].insert(arg.second);
        }

        auto&& merger = TermReplaceTransformer(FactoryNest(), termMap);
        auto stateInfoSet = it.second;
        it.second.clear();

        for (auto&& stateInfo : stateInfoSet) {
            auto&& transformedState = merger.transform(stateInfo.state);
            it.second.insert({transformedState, args});
        }
    }*/
}

char ContractManager::ID = 0;
ContractManager::ContractStates ContractManager::states;
ContractManager::ContractArguments ContractManager::arguments;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
