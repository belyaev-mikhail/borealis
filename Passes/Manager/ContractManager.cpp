/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include "Passes/Manager/ContractManager.h"

namespace borealis {

ContractManager::ContractManager() :
        ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module& M) {
    return false;
}

void ContractManager::addContract(llvm::Function* F, PredicateState::Ptr S) {
    if(!S->isEmpty()) data[F].push_back(S);
}

void ContractManager::print(llvm::raw_ostream& st, const llvm::Module* M) const {
    for(auto&& it : data) {
        errs()<<"Function "<<it.first->getName()<<endl;
        for(auto&& s_it : it.second) {
        	errs()<<s_it<<endl;
        }
    }
}


char ContractManager::ID = 0;
ContractManager::ContractData ContractManager::data = ContractData();

static llvm::RegisterPass<ContractManager>
X("contract-manager", "contract manager pass", false, false);


} /* namespace borealis */



