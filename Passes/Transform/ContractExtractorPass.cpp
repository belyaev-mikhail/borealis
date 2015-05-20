/*
 * ContractExtractorPass.cpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#include "Passes/Transform/ContractExtractorPass.h"

#include "State/Transformer/ContractExtractorTransform.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Logging/logger.hpp"

namespace borealis {

ContractExtractorPass::ContractExtractorPass() : ProxyFunctionPass(ID) {}

bool ContractExtractorPass::runOnFunction(llvm::Function& F) {
	for(auto it = F.begin(), ite = F.end(); it != ite; ++it) {
		for(auto i_it = it->begin(), i_ite = it->end(); i_it != i_ite; ++i_it) {
			if(llvm::isa<llvm::CallInst>(i_it)) {
				GetAnalysis<PredicateStateAnalysis>::doit(this, F).getInstructionState(i_it);
			}
		}
	}
	return false;
}

void ContractExtractorPass::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
	Info.setPreservesAll();

	AUX<PredicateStateAnalysis>::addRequiredTransitive(Info);
}

char ContractExtractorPass::ID = 0;

static llvm::RegisterPass<ContractExtractorPass>
X("contract-extractor", "contract extractor pass", false, false);

} /* namespace borealis */


