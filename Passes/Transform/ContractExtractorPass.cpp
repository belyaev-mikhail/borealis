/*
 * ContractExtractorPass.cpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#include "Passes/Transform/ContractExtractorPass.h"

#include "../../State/Transformer/ContractExtractorTransform.h"

namespace borealis {

ContractExtractorPass::ContractExtractorPass() : llvm::FunctionPass(ID) {}

bool ContractExtractorPass::runOnFunction(llvm::Function& F) {
	return false;
}

void ContractExtractorPass::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
	Info.setPreservesAll();
}

char ContractExtractorPass::ID = 0;

static llvm::RegisterPass<ContractExtractorPass>
X("contract-extractor", "contract extractor pass", false, false);

} /* namespace borealis */


