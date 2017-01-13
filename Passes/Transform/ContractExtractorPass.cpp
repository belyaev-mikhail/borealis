/*
 * ContractExtractorPass.cpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */
#include <llvm/Analysis/AliasAnalysis.h>

#include "Codegen/llvm.h"
#include "ContractExtractorPass.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "State/Transformer/ContractExtractorTransformer.h"
#include "State/Transformer/EqualityMapper.h"
#include "State/Transformer/FindRTVEquiv.h"
#include "State/Transformer/FunctionSummariesTransformer.h"
#include "State/Transformer/ReplaceTermTransformer.h"
#include "State/Transformer/StateRipper.h"
#include "State/Transformer/StateSlicer.h"
#include "State/Transformer/TerTermToPathPredTransf.h"
#include "State/Transformer/UnexpPathPrDeleter.h"
#include "State/Transformer/UnusedGlobalsDeleter.h"
#include "State/Transformer/UnusedVariablesDeleter.h"


namespace borealis {

ContractExtractorPass::ContractExtractorPass() : ProxyFunctionPass(ID) {}

bool ContractExtractorPass::runOnFunction(llvm::Function& F) {
    FN = FactoryNest(F.getDataLayout(), GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(&F));
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

    if (!F.doesNotReturn()) {
        PredicateState::Ptr S;
        auto&& ret=llvm::getAllRets(&F);
        if(ret.size()==0)
            return false;
        assert(ret.size()==1);
        S = PSA->getInstructionState(*ret.begin());
        auto&& fName=std::string(F.getName());
        auto&& ttTransf= TerTermToPathPredTransf(FN);
        auto&& onlyPath = ttTransf.transform(S);
        auto&& mapper = EqualityMapper(FN);
        auto&& mappedState = mapper.transform(onlyPath);
        auto&& mapping = mapper.getMappedValues();
        auto&& sliced = StateSlicer(FN, TermSet({FN.Term->getReturnValueTerm(&F)})).transform(mappedState);
        auto&& rtv = FN.Term->getReturnValueTerm(&F);
        if(mapping.find(rtv)==mapping.end()){
            return false;
        }
        auto&& rtvmap=FindRTVEquiv(FN,rtv);
        rtvmap.transform(sliced->reverse());
        auto&& rtvEq = rtvmap.getRtvEquiv();
        Term::Ptr rtvMap=mapping.at(rtv);
        auto&& extractor = FunctionSummariesTransformer(FN, rtvEq);
        extractor.transform(sliced);
        auto&& isImpl = extractor.getIsImpl();
        if(isImpl){
            CM->addSummary(&F,sliced,*FM);
            return false;
        }
        auto&& terms = extractor.getTermSet();
        auto&& protPredsMapping = extractor.getProtPredMapping();
        auto&& protPreds = extractor.getProtectedPredicates();
        if (terms.size() == 0)
            return false;
        for (auto&& i = 0U; i < protPreds.size(); ++i) {
            auto&& deleter = StateRipper(FN, protPreds[i]);
            auto&& del = deleter.transform(sliced);
            auto&& sliced1 = StateSlicer(FN, terms[i]).transform(del);
            auto&& withoutUnused = UnusedVariablesDeleter(FN, terms[i]).transform(sliced1);
            auto&& del2 = UnexpPathPrDeleter(FN, protPreds[i]).transform(withoutUnused);
            auto&& withoutGlob=UnusedGlobalsDeleter(FN,terms[i]).transform(del2);
            auto&& sliced2 = StateSlicer(FN, terms[i]).transform(withoutGlob);
            auto&& result = ReplaceTermTransformer(FN, fName).transform(sliced2);
            if (auto&& k = util::at(protPredsMapping, protPreds[i])) {
                auto&& eq=FN.Predicate->getEqualityPredicate(rtv,k.getUnsafe());
                auto&& pr=FN.State->Imply(result,eq);
                CM->addSummary(&F,pr,*FM);
            }
        }
    }
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

    CM->addContract(I.getCalledFunction(), *FM, transformedState, argToTerms);
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