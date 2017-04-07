
#ifndef SUMMARIESEXTRACTOR_HPP_
#define SUMMARIESEXTRACTOR_HPP_

#include <llvm/IR/Instructions.h>

#include "State/Transformer/ReplaceTermTransformer.h"
#include "State/Transformer/StateSlicer.h"
#include "TermCollector.h"
#include "Util/macros.h"

#include <vector>

namespace borealis{

template <class Pass>
class SummariesExtractor {

    using FunVec = std::vector<std::pair<const llvm::Function*, int>>;
    using TermSet = std::unordered_set<Term::Ptr, TermHash, TermEquals>;

public:

    SummariesExtractor(const FactoryNest& FN, const llvm::Instruction* i, PredicateState::Ptr quer, Pass* pass, PredicateState::Ptr funState)
            : FN(FN), I(i), pass(pass), funState(funState) {
        TF = borealis::TypeFactory::get();
        query = quer;
        auto&& fun = I->getParent()->getParent();
        //resulting = pass->getFunctionState(fun);
        //getFunUsages(*fun, 0);
        auto&& tc = TermCollector(FN);
        tc.transform(quer);
        TermSet terms;
        for(auto&& it : tc.getTerms())
            if(isInterestingTerm(it))
                terms.insert(it);
        auto&& collector = TermCollector(FN);
        collector.transform(StateSlicer(FN, terms).transform(funState));
        auto&& inters = collector.getTerms();
        isFirstArg = true;

        auto&& tc2 = TermCollector(FN);
        tc2.transform(quer);
        funState = StateSlicer(FN, tc2.getTerms()).transform(funState);


        for (auto&& arg : fun->getArgumentList()){
            auto&& argTerm = FN.Term->getArgumentTerm(&arg);
            if (util::contains(inters, argTerm)){
                curArg = argTerm;
                getFunUsages(*fun, arg.getArgNo(), FN.State->Basic());
            }
            if (isFirstArg) {
                resulting = FN.State->Choice(resChoices);
                isFirstArg = false;
            }
            else {
                if(auto&& choice = llvm::dyn_cast<PredicateStateChoice>(resulting)){
                    ASSERT(resChoices.size() == choice->getChoices().size(), "Vectors sizes must be equal");
                    std::vector<PredicateState::Ptr> newChoices;
                    for(auto i = 0U; i < choice->getChoices().size(); ++i){
                        newChoices.push_back(FN.State->Chain(choice->getChoices()[i], resChoices[i]));
                    }
                    resulting = FN.State->Choice(newChoices);
                }
            }
            resulting = ReplaceTermTransformer(FN, usedArgs, argTerm).transform(resulting);
            usedArgs.clear();
            resChoices.clear();
            query = ReplaceTermTransformer(FN, argTerm, curArg).transform(query);
            resulting = ReplaceTermTransformer(FN, argTerm, curArg).transform(resulting);
            funState = ReplaceTermTransformer(FN, argTerm, curArg).transform(funState);
        }
        resulting = FN.State->Chain(resulting, funState);
    }

    PredicateState::Ptr getResultingPS() {return resulting;}
    PredicateState::Ptr getQuery(){return query;}

private:

    void endPoint(PredicateState::Ptr curRes, const llvm::Function& F){

        auto&& tc = TermCollector(FN);
        tc.transform(curRes);
        if (tc.getTerms().size() != 0 && isFirstArg){  /*Add free_var only for first arg */
            auto&& freeTerm = FN.Term->getFreeVarTerm(TF->getInteger(32), "free_var");
            auto&& constTerm = FN.Term->getConstTerm(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(F.getContext()), resChoices.size()));
            auto&& path = FN.Predicate->getEqualityPredicate(freeTerm, constTerm, Locus(), PredicateType::PATH);
            curRes = FN.State->Chain(FN.State->Basic(std::vector<Predicate::Ptr>{path}), curRes);
        }
        resChoices.push_back(curRes);
        return;
    }

    void getFunUsages(const llvm::Function& F, const unsigned int numArg, PredicateState::Ptr curRes) {
        usedArgs.insert(curArg);
        auto&& sliced = getArgUsagesForFun(F, curArg);
        curRes = FN.State->Chain(sliced, curRes);
        if (F.users().begin() == F.users().end()) {
            endPoint(curRes, F);
            return;
        }
        for (auto&& it : F.users()) {
            if (auto&& call = llvm::dyn_cast<llvm::CallInst>(it)) {
                auto&& fun = call->getParent()->getParent();
                auto&& tmp = FN.Term->getValueTerm(call->getArgOperand(numArg));
                if (isInterestingTerm(tmp)){
                    curArg = FN.Term->getValueTerm(call->getArgOperand(numArg));
                }
                else {
                    auto&& eq = FN.Predicate->getEqualityPredicate(curArg, tmp);
                    curRes = curRes + eq;
                    endPoint(curRes, F);
                    continue;
                }
                int i = 0;
                for (auto&& arg : fun->getArgumentList()){
                    auto&& argTerm = FN.Term->getValueTerm(&arg);
                    if (curArg->equals(argTerm.get()))
                        break;
                    ++i;
                }
                getFunUsages(*fun, i, curRes);
            }
        }
        return;
    }


    PredicateState::Ptr getArgUsagesForFun(const llvm::Function& F, const Term::Ptr arg) {
        auto&& ps = pass->getFunctionState(&F);
        return StateSlicer(FN, TermSet{arg}).transform(ps);
    }

    bool isInterestingTerm(Term::Ptr term) {
        return not (llvm::is_one_of<
                OpaqueBoolConstantTerm,
                OpaqueIntConstantTerm,
                OpaqueFloatingConstantTerm,
                OpaqueStringConstantTerm,
                OpaqueNullPtrTerm
        >(term)) && (term->getNumSubterms() == 0);
    }


    const FactoryNest FN;
    const llvm::Instruction* I;
    PredicateState::Ptr query;
    Pass* pass;
    PredicateState::Ptr funState;
    PredicateState::Ptr resulting;
    Term::Ptr curArg;
    std::vector<PredicateState::Ptr> resChoices;
    TermSet usedArgs;

    borealis::TypeFactory::Ptr TF;
    bool isFirstArg;

};

} /* namespace borealis */

#include "Util/unmacros.h"
#endif /* SUMMARIESEXTRACTOR_HPP_ */