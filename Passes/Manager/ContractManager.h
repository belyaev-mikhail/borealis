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
public:
    using ContractData = std::unordered_map<llvm::Function*, std::vector<PredicateState::Ptr>>;

    static char ID;
    static ContractData data;

    ContractManager();
    virtual ~ContractManager() = default;

    virtual bool runOnModule(llvm::Module& M) override;

    void addContract(llvm::Function* F, PredicateState::Ptr S);
    virtual void print(llvm::raw_ostream& st, const llvm::Module* M) const;

};

} /* namespace borealis */

#endif /* CONTRACTMANAGER_H_ */
