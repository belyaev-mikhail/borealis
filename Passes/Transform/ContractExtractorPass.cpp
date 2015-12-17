/*
 * ContractExtractorPass.cpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */
#include <llvm/Analysis/AliasAnalysis.h>
#include <State/Transformer/UnnecesPredDeleter.h>

#include "Passes/Tracker/SlotTrackerPass.h"
#include "State/Transformer/EqualityMapper.h"
#include "State/Transformer/ChoiceInfoCollector.h"
#include "State/Transformer/ContractExtractorTransformer.h"
#include "State/Transformer/EqualityMapper.h"
#include "State/Transformer/FunctionSummariesTransformer.h"
#include "State/Transformer/StateSlicer.h"
#include "Term/TermFactory.h"
#include "ContractExtractorPass.h"


namespace borealis {

ContractExtractorPass::ContractExtractorPass() : ProxyFunctionPass(ID) {}

bool ContractExtractorPass::runOnFunction(llvm::Function& F) {
    FN = FactoryNest(GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(&F));
    FM = &GetAnalysis<FunctionManager>::doit(this, F);
    CM = &GetAnalysis<ContractManager>::doit(this, F);
    PSA = &GetAnalysis<PredicateStateAnalysis>::doit(this, F);
    AA = GetAnalysis<llvm::AliasAnalysis>::doit(this,F);

    for (auto&& I : util::viewContainer(F)
                    .flatten()
                    .map(ops::take_pointer)
                    .map(llvm::dyn_caster<llvm::CallInst>())
                    .filter()) {
        processCallInstruction(*I, PSA->getInstructionState(I));
	}

	/*if (not F.getName().equals("main")) {
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
        TermSet TS;
        TS.insert(FN.Term->getReturnValueTerm(&F));
        auto&& sliced1 = StateSlicer(FN, TS, &AA).transform(mappedState);
        auto&& choiceInfo = ChoiceInfoCollector(FN);
        choiceInfo.transform(sliced1);
        choiceInfo.pushBackTemp();//add last path
        auto&& vec = choiceInfo.getChoiceInfo();
        auto&& rtv = FN.Term->getReturnValueTerm(&F);
        auto&& extractor = FunctionSummariesTransformer(FN, mapping, vec, rtv);
        extractor.transform(sliced1);
        auto&& terms = extractor.getTermSet();
        if (terms.size() == 0)
            return false;
        auto&& sliced = StateSlicer(FN, terms, &AA).transform(mappedState);
        auto&& protStates = extractor.getProtectedPredicates();
        auto&& choiceInfo2 = ChoiceInfoCollector(FN);
        choiceInfo2.transform(sliced);
        vec = choiceInfo.getChoiceInfo();
        auto&& deleter = UnnecesPredDeleter(FN, protStates, terms, F.getArgumentList());
        auto&& result = deleter.transform(sliced);
        auto&& result2 = StateSlicer(FN, deleter.getRigthTerms(), &AA).transform(result);
        if (not result2->isEmpty())
            errs() << result2 << endl;
        //if (not argToTerms.empty()) {
        //    CM->addSummary(&F, FN, transformedState, argToTerms);
        //}
    }*/
    return false;
}

void ContractExtractorPass::processCallInstruction(llvm::CallInst& I, PredicateState::Ptr S) {
    if (I.getCalledFunction() == nullptr) return;

    auto&& mapper = EqualityMapper(FN);
    auto&& mappedState = mapper.transform(S);
    auto&& mapping = mapper.getMappedValues();

    auto&& extractor = ContractExtractorTransformer(FN, I, mapping);
    auto&& transformedState = extractor.transform(mappedState);
    auto&& argToTerms = extractor.getArgToTermMapping();

    CM->addContract(I.getCalledFunction(), FN, *FM, transformedState, argToTerms);
}

void ContractExtractorPass::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
    Info.setPreservesAll();

    AUX<FunctionManager>::addRequiredTransitive(Info);
    AUX<ContractManager>::addRequiredTransitive(Info);
    AUX<PredicateStateAnalysis>::addRequiredTransitive(Info);
    AUX<SlotTrackerPass>::addRequiredTransitive(Info);
    AUX<llvm::AliasAnalysis>::addRequiredTransitive(Info);
}

char ContractExtractorPass::ID = 0;

static llvm::RegisterPass<ContractExtractorPass>
X("contract-extractor", "Contract extractor pass", false, false);


} /* namespace borealis */
