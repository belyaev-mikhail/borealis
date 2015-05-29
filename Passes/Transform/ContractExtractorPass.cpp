/*
 * ContractExtractorPass.cpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#include "Passes/Transform/ContractExtractorPass.h"

#include "State/Transformer/EqualityMapper.h"
#include "State/Transformer/ContractExtractorTransformer.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "Passes/Manager/ContractManager.h"
#include "Logging/logger.hpp"

namespace borealis {

ContractExtractorPass::ContractExtractorPass() : ProxyFunctionPass(ID) {}

bool ContractExtractorPass::runOnFunction(llvm::Function& F) {
    GetAnalysis<SlotTrackerPass>::doit(this, F).runOnModule(*F.getParent());
	auto&& st = GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(&F);
	FN = FactoryNest(st);
	for(auto it = F.begin(), ite = F.end(); it != ite; ++it) {
	    for(auto i_it = it->begin(), i_ite = it->end(); i_it != i_ite; ++i_it) {
		    if(auto&& I = llvm::dyn_cast<llvm::CallInst>(i_it)) {
			    auto&& s = GetAnalysis<PredicateStateAnalysis>::doit(this, F).getInstructionState(i_it);
				processCallInstruction(*I, s);
			}
		}
	}
	return false;
}

void ContractExtractorPass::processCallInstruction(llvm::CallInst& I, borealis::PredicateState::Ptr S) {
    if(auto&& chain = llvm::dyn_cast<PredicateStateChain>(S.get())) {
        S = chain->getCurr();
    }
    auto&& mapper = EqualityMapper();
    auto&& mappedState = mapper.transform(S);
    auto&& mapping = mapper.getMappedValues();
    auto&& extractor = ContractExtractorTransformer(FN, I, mapping);
    auto&& transformedState = extractor.transform(mappedState);
    auto&& termsToArg = extractor.getTermToArgMapping();
    GetAnalysis<ContractManager>::doit(this, *I.getParent()->getParent()).addContract(I.getCalledFunction(), transformedState, termsToArg);
}

void ContractExtractorPass::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
    Info.setPreservesAll();

    AUX<PredicateStateAnalysis>::addRequiredTransitive(Info);
    AUX<SlotTrackerPass>::addRequiredTransitive(Info);
    AUX<ContractManager>::addRequiredTransitive(Info);
}

char ContractExtractorPass::ID = 0;

static llvm::RegisterPass<ContractExtractorPass>
X("contract-extractor", "contract extractor pass", false, false);

} /* namespace borealis */

