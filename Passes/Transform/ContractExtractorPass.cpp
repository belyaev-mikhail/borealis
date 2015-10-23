/*
 * ContractExtractorPass.cpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#include "Passes/Tracker/SlotTrackerPass.h"
#include "State/Transformer/EqualityMapper.h"
#include "State/Transformer/ChoiceInfoCollector.h"
#include "State/Transformer/ContractExtractorTransformer.h"
#include "State/Transformer/EqualityMapper.h"
#include "State/Transformer/FunctionSummariesTransformer.h"
#include "Term/TermFactory.h"
#include "ContractExtractorPass.h"

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

	if (not F.getName().equals("main")) {
        PredicateState::Ptr S;
        for (auto &&I : util::viewContainer(F)
                        .flatten()
                        .map(ops::take_pointer)
                        .filter()) {
            if (I->isTerminator()) {
                S = PSA->getInstructionState(I);
            }
        }
        auto&& mapper = EqualityMapper(FN);
        auto&& mappedState = mapper.transform(S);
        auto&& mapping = mapper.getMappedValues();

        auto&& choiceInfo = ChoiceInfoCollector(FN);
        choiceInfo.transform(mappedState);
        auto&& vec = choiceInfo.getChoiceInfo();

        auto&& rtv = FN.Term->getReturnValueTerm(&F);
        auto&& extractor = FunctionSummariesTransformer(FN, F.getArgumentList(), mapping, vec, rtv);
        auto&& transformedState = extractor.transform(mappedState);
        auto&& argToTerms = extractor.getArgToTermMapping();

        if (not argToTerms.empty()) {
            CM->addSummary(&F, FN, transformedState, argToTerms);
        }
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
