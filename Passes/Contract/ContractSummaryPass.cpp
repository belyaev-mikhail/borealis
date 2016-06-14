//
// Created by kivi on 28.01.16.
//

#include "ContractSummaryPass.h"
#include "Util/passes.hpp"

namespace borealis {

ContractSummaryPass::ContractSummaryPass() : ModulePass(ID) {}

bool ContractSummaryPass::runOnModule(llvm::Module&) {
    CM = &GetAnalysis<ContractManager>::doit(this);
    CM->printResults();
    return false;
}

void ContractSummaryPass::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
    Info.setPreservesAll();

    AUX<ContractManager>::addRequiredTransitive(Info);
}

char ContractSummaryPass::ID = 0;
static llvm::RegisterPass<ContractSummaryPass>
X("contract-summary", "Contract summary pass", false, false);

} /* namespace borealis */
