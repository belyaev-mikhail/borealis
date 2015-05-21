/*
 * ContractManager.h
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#ifndef CONTRACTMANAGER_H_
#define CONTRACTMANAGER_H_

#include <llvm/Pass.h>

#include <unordered_map>
#include "Logging/logger.hpp"
#include "State/PredicateState.h"

namespace borealis {

class ContractManager : public llvm::ModulePass {

    using Manager = std::unordered_map<llvm::Function*, std::vector<PredicateState::Ptr>>;

public:

    static char ID;

    ContractManager();
    virtual ~ContractManager() = default;

    virtual bool runOnModule(llvm::Module& M) override;

    void addContract(llvm::Function* F, PredicateState::Ptr S);
    virtual void print(llvm::raw_ostream& st, const llvm::Module* M) const;

private:

    Manager manager;
};

} /* namespace borealis */

#endif /* CONTRACTMANAGER_H_ */
