/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include "Passes/Manager/ContractManager.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module&) {
    return false;
}

void ContractManager::addContract(llvm::Function* F, PredicateState::Ptr S, const ArgToTerms& mapping) {
    if (not S->isEmpty()) {
        data[F].insert({S, mapping});
    }
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {

    auto&& dbg = dbgs();

    for (auto&& it : data) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;

        for (auto&& stateInfo : it.second) {
            dbg << "Arguments:" << endl;
            dbg << stateInfo.mapping << endl;
            dbg << "State:" << endl;
            dbg << stateInfo.state << endl;
        }
        dbg << endl;
    }

    dbg << end;
}

char ContractManager::ID = 0;
ContractManager::ContractData ContractManager::data;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
