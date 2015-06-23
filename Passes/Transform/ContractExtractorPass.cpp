/*
 * ContractExtractorPass.cpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#include "ContractExtractorPass.h"
#include "../Tracker/SlotTrackerPass.h"
#include "../../Util/functional.hpp"
#include "../../Util/util.hpp"
#include "../../State/Transformer/EqualityMapper.h"
#include "../../State/Transformer/ContractExtractorTransformer.h"

namespace borealis {

ContractExtractorPass::ContractExtractorPass() : ProxyFunctionPass(ID) {}

bool ContractExtractorPass::runOnFunction(llvm::Function& F) {
    FN = FactoryNest(GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(&F));
    CM = &GetAnalysis<ContractManager>::doit(this, F);
    PSA = &GetAnalysis<PredicateStateAnalysis>::doit(this, F);

    for (auto&& I : util::viewContainer(F)
                    .flatten()
                    .map(ops::take_pointer)
                    .map(llvm::dyn_caster<llvm::CallInst>())
                    .filter()) {
        processCallInstruction(*I, PSA->getInstructionState(I));
	}
	return false;
}

void ContractExtractorPass::processCallInstruction(llvm::CallInst& I, PredicateState::Ptr S) {
    auto&& mapper = EqualityMapper(FN);
    auto&& mappedState = mapper.transform(S);
    auto&& mapping = mapper.getMappedValues();

    auto&& extractor = ContractExtractorTransformer(FN, I, mapping);
    auto&& transformedState = extractor.transform(mappedState);
    auto&& argToTerms = extractor.getArgToTermMapping();

    CM->addContract(I.getCalledFunction(), FN, transformedState, argToTerms);
}

void ContractExtractorPass::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
    Info.setPreservesAll();

    AUX<ContractManager>::addRequiredTransitive(Info);
    AUX<PredicateStateAnalysis>::addRequiredTransitive(Info);
    AUX<SlotTrackerPass>::addRequiredTransitive(Info);
}

char ContractExtractorPass::ID = 0;

static llvm::RegisterPass<ContractExtractorPass>
X("contract-extractor", "Contract extractor pass", false, false);

} /* namespace borealis */
