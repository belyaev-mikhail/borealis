//
// Created by kivi on 28.01.16.
//

#ifndef BOREALIS_CONTRACTSUMMARYPASS_H
#define BOREALIS_CONTRACTSUMMARYPASS_H

#include <llvm/Pass.h>

#include "ContractManager.h"

namespace borealis {

class ContractSummaryPass : public llvm::ModulePass {

public:

    static char ID;

    ContractSummaryPass();
    virtual ~ContractSummaryPass()  = default;

    virtual bool runOnModule(llvm::Module&) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;

private:

    ContractManager* CM;

};

} /* namespace borealis */

#endif //BOREALIS_CONTRACTSUMMARYPASS_H
