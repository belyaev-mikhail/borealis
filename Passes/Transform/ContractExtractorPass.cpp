/*
 * ContractExtractorPass.cpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */
#include <llvm/Analysis/AliasAnalysis.h>

#include "Codegen/llvm.h"
#include "ContractExtractorPass.h"
#include "Passes/Manager/FunctionManager.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "SMT/Z3/Z3.h"
#include "SMT/Z3/Solver.h"
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

void ContractExtractorPass::getFunUsages(llvm::Function &F) {
    for(auto&& it : F.users()) {
        if (auto&& call = llvm::dyn_cast<llvm::CallInst>(it)){
            auto&& fun = call->getParent()->getParent();
            errs()<<"FUN="<<fun->getName()<<endl;
            usages.insert(fun);
            getFunUsages(*fun);
        }
    }
}


bool ContractExtractorPass::runOnFunction(llvm::Function& F) {
    FN = FactoryNest(F.getDataLayout(), GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(&F));
    FM = &GetAnalysis<FunctionManager>::doit(this, F);
    CM = &GetAnalysis<ContractManager>::doit(this, F);
    PSA = &GetAnalysis<PredicateStateAnalysis>::doit(this, F);
    AA = GetAnalysis<llvm::AliasAnalysis>::doit(this, F);
    CG = &GetAnalysis<llvm::CallGraphWrapperPass>::doit(this, F).getCallGraph();

    //CG->print(llvm::errs());
    //usages.clear();
    //getFunUsages(F);

    for (auto&& I : util::viewContainer(F)
            .flatten()
            .map(ops::take_pointer)
            .map(llvm::dyn_caster<llvm::CallInst>())
            .filter()) {
        processCallInstruction(*I, PSA->getInstructionState(I));
    }

    if (/*!F.doesNotReturn()*/ true) {
        errs()<<"Name="<<F.getName()<<endl<<endl<<endl;
        auto&& ret=llvm::getAllRets(&F);
        if(ret.size()==0)
            return false;
        assert(ret.size()==1);

        /*for(auto&& it : usages){
            errs()<<it->getName()<<endl;
            auto&& re = llvm::getAllRets(it);
            if(re.size()==0)
                return false;
            assert(re.size() == 1);
            PredicateStateAnalysis* PSA2 = &GetAnalysis<PredicateStateAnalysis>::doit(this, *it);
            errs()<< PSA2->getInstructionState(*re.begin()) << endl;
        };*/



        PredicateState::Ptr S = PSA->getInstructionState(*ret.begin());
        errs()<<"S="<<S<<endl;
        auto&& fName=std::string(F.getName());
        auto&& ttTransf= TerTermToPathPredTransf(FN);
        auto&& onlyPath = ttTransf.transform(S);
       /* auto&& mapper = EqualityMapper(FN);
        auto&& mappedState = mapper.transform(onlyPath);
        auto&& mapping = mapper.getMappedValues();*/

        auto&& mappedState = onlyPath;  //!!!!!!!!!!!!!!!

        //errs()<<"EqualityMapped\n";
        TermSet ts;
        auto&& rtv = FN.Term->getReturnValueTerm(&F);
        ts.insert(rtv);
        for(auto&& it = F.arg_begin(); it != F.arg_end();++it){
            ts.insert(FN.Term->getArgumentTerm(it));
        }
        std::vector<PredicateState::Ptr> summaries;
        for(auto&& it : ts) {
            //errs()<<"It="<<it<<endl;
            //if (rtv->equals(it.get()) && mapping.find(rtv)==mapping.end()) { continue;}
            auto&& argEq = FindRTVEquiv(FN, it);
            argEq.transform(mappedState);
            auto&& extractor = FunctionSummariesTransformer(FN, argEq.getRtvEquiv(), rtv);
            extractor.transform(mappedState);
            auto&& isImpl = extractor.getIsImpl();
            if(isImpl){
                CM->addSummary(&F, mappedState, *FM);
                return false;
            }
            auto&& rtvValues = extractor.getRtvValues();
            if(rtvValues.size() == 0)
                continue;
            std::vector<PredicateState::Ptr> choices;
            for (auto&& it : rtvValues){
                auto&& bas = FN.State->Basic(std::vector<Predicate::Ptr>{it});
                choices.push_back(bas);
            }
            //errs()<<"Summary="<<FN.State->Choice(choices)<<endl;
            //CM->addSummary(&F, FN.State->Choice(choices), *FM);
            /*auto&& terms = extractor.getTermSet();
            auto&& protPredsMapping = extractor.getProtPredMapping();
            auto&& protPreds = extractor.getProtectedPredicates();
            if (terms.size() == 0)
                continue;
            for (auto&& i = 0U; i < protPreds.size(); ++i) {
                auto&& deleter = StateRipper(FN, protPreds[i]);
                auto&& del = deleter.transform(mappedState);
                TermSet t;
                t.insert(terms[i].begin(), terms[i].end());
                t.insert(rtv);
                auto&& sliced1 = StateSlicer(FN, t).transform(del);
                //auto&& withoutUnused = UnusedVariablesDeleter(FN, terms[i]).transform(sliced1);
                //auto&& del2 = UnexpPathPrDeleter(FN, protPreds[i]).transform(withoutUnused);
                auto&& withoutGlob = UnusedGlobalsDeleter(FN,terms[i]).transform(sliced1);
                auto&& sliced2 = StateSlicer(FN, t).transform(withoutGlob);
                //auto&& result = ReplaceTermTransformer(FN, fName).transform(sliced2);
                if (auto&& k = util::at(protPredsMapping, protPreds[i])) {
                    Predicate::Ptr resPred;
                    if(rtv->equals(it.get())){
                        resPred = FN.Predicate->getEqualityPredicate(it, k.getUnsafe());
                    }
                    else {
                        resPred = FN.Predicate->getStorePredicate(it, k.getUnsafe());
                    }
                    auto&& pr = FN.State->Imply(sliced2, resPred);
                    Z3::ExprFactory ef;
                    Z3::Solver s(ef, FM->getMemoryBounds(&F).first, FM->getMemoryBounds(&F).second);
                    auto res = s.checkSummary(pr, S);
                    errs()<<"RESULT="<<res<<endl;
                    if(res.isUnsat()){
                        auto&& withoutUnused = UnusedVariablesDeleter(FN, terms[i]).transform(sliced2);
                        errs()<<"Summary=="<<withoutUnused<<endl;
                        errs()<<"Chain="<<FN.State->Chain(withoutUnused, resPred);
                        //CM->addSummary(&F,FN.State->Imply(withoutUnused, resPred),*FM);
                        summaries.push_back(FN.State->Chain(withoutUnused, resPred));
                        //CM->addSummary(&F,FN.State->Chain(withoutUnused, resPred),*FM);
                    }
                }
            }*/
        }

        /*if(mapping.find(rtv)==mapping.end() && args.size() == 0){
            return false;
        }*/


    }
    return false;
}

void ContractExtractorPass::processCallInstruction(llvm::CallInst& I, PredicateState::Ptr S) {
    /*if (I.getCalledFunction() == nullptr) return;

    auto&& mapper = EqualityMapper(FN);
    auto&& mappedState = mapper.transform(S);
    auto&& mapping = mapper.getMappedValues();

    auto&& extractor = ContractExtractorTransformer(FN, I, mapping);
    auto&& transformedState = extractor.transform(mappedState);
    auto&& argToTerms = extractor.getArgToTermMapping();

    CM->addContract(I.getCalledFunction(), *FM, transformedState, argToTerms);*/
}

void ContractExtractorPass::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
    Info.setPreservesAll();

    AUX<FunctionManager>::addRequiredTransitive(Info);
    AUX<ContractManager>::addRequiredTransitive(Info);
    AUX<PredicateStateAnalysis>::addRequiredTransitive(Info);
    AUX<SlotTrackerPass>::addRequiredTransitive(Info);
    AUX<llvm::AliasAnalysis>::addRequiredTransitive(Info);
    AUX<llvm::CallGraphWrapperPass>::addRequiredTransitive(Info);
}

char ContractExtractorPass::ID = 0;

static llvm::RegisterPass<ContractExtractorPass>
        X("contract-extractor", "Contract extractor pass", false, false);


} /* namespace borealis */