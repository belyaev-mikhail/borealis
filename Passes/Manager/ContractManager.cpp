/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include "Passes/Manager/ContractManager.h"
#include <algorithm>

namespace borealis {

ContractManager::ContractManager() :
        ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module& M) {
    return false;
}

void ContractManager::addContract(llvm::Function* F, PredicateState::Ptr S, ArgsToTerm& mapping) {
    if(!S->isEmpty()) {
        data[F].insert({S, mapping});
    }
}

void ContractManager::print(llvm::raw_ostream& st, const llvm::Module* M) const {
    for(auto&& it : data) {
        errs()<<endl<<"---"<<"Function "<<it.first->getName()<<"---"<<endl;
        for(auto&& s_it : it.second) {
            for(auto&& args : s_it.mapping) {
                errs()<<args.first<<": ";
                for(auto&& ait : args.second) {
                    errs()<<ait<<", ";
                }
                errs()<<endl;
            }
        	errs()<<s_it.state<<endl;
        }
        errs()<<endl;
    }
}


char ContractManager::ID = 0;
ContractManager::ContractData ContractManager::data = ContractData();

static llvm::RegisterPass<ContractManager>
X("contract-manager", "contract manager pass", false, false);


} /* namespace borealis */



