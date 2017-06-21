
#ifndef SUMMARIESEXTRACTOR_HPP_
#define SUMMARIESEXTRACTOR_HPP_

#include <llvm/IR/ValueMap.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include "State/Transformer/CallsSummariesExtraction.hpp"
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
    using TermVec = std::vector<Term::Ptr>;

public:

    SummariesExtractor(const FactoryNest& FN, const llvm::Instruction* i, PredicateState::Ptr quer, Pass* pass, PredicateState::Ptr funState)
            : FN(FN), I(i), pass(pass) {
        TF = borealis::TypeFactory::get();
        query = quer;
        isFirstArg = true;
        funcState = funState;
        auto&& fun = I->getParent()->getParent();
        if (fun->getArgumentList().size() == 0) {
            resulting = funState;
            return;
        }
        auto&& tc = TermCollector(FN);
        tc.transform(quer);
        TermSet terms;
        for(auto&& it : tc.getTerms())
            if(isInterestingTerm(it))
                terms.insert(it);
        auto&& collector = TermCollector(FN);
        collector.transform(funState);
        auto&& inters = collector.getTerms();
        TermVec argsNo;
        for (auto&& arg : fun->getArgumentList()){
            auto&& argTerm = FN.Term->getArgumentTerm(&arg);
            if (util::contains(inters, argTerm)) {
                auto&& term = FN.Term->getValueTerm(argTerm->getType(), arg.getName().str());
                query = ReplaceTermTransformer(FN, argTerm, term).transform(query);
                argsNo.push_back(argTerm);
            }
        }
        getFunUsages(*fun, argsNo, FN.State->Basic(), argsNo, nullptr);
        if(resChoices.size()==1)
            resulting = resChoices[0];
        else
            resulting = FN.State->Choice(resChoices);
        tc = TermCollector(FN);
        tc.transform(resulting);
        for(auto&& it : tc.getTerms()){
            if(llvm::isa<borealis::ArgumentTerm>(it)){
                auto&& term = FN.Term->getValueTerm(it->getType(), it->getName());
                resulting = ReplaceTermTransformer(FN, it, term).transform(resulting);
            }

        }
        //errs()<<"resulting="<<resulting<<endl;
    }

    PredicateState::Ptr getResultingPS() {return resulting;}
    PredicateState::Ptr getQuery(){return query;}

private:

    void endPoint(PredicateState::Ptr curRes, const llvm::Function& F){

        auto&& tc = TermCollector(FN);
        tc.transform(curRes);
        if (tc.getTerms().size() != 0 ){
            auto&& freeTerm = FN.Term->getFreeVarTerm(TF->getInteger(32), "free_var");
            auto&& constTerm = FN.Term->getConstTerm(llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(F.getContext()), resChoices.size()));
            auto&& path = FN.Predicate->getEqualityPredicate(freeTerm, constTerm, Locus(), PredicateType::PATH);
            curRes = FN.State->Chain(FN.State->Basic(std::vector<Predicate::Ptr>{path}), curRes);
        }
        resChoices.push_back(curRes);
        return;
    }

    PredicateState::Ptr getStateIfArgIsFun(llvm::CallInst* inst, Term::Ptr retArg){
        auto&& res = FN.State->Basic();
        auto&& calledFun = inst->getCalledFunction();
        FN.Slot->incorporateFunction(calledFun);
        TermVec calledArgs;
        int i = 0;
        for(auto&& arg : calledFun->getArgumentList()) {
            auto&& tmp = FN.Term->getValueTerm(inst->getArgOperand(i));
            auto&& argTerm = FN.Term->getValueTerm(&arg);
            calledArgs.push_back(argTerm);
            ++i;
            auto&& eq = FN.Predicate->getEqualityPredicate(argTerm, tmp);
            res = FN.State->Chain(res, FN.State->Basic(std::vector<Predicate::Ptr>{eq}));
        }
        auto&& ret=llvm::getAllRets(calledFun);
        if(ret.size()==1){
            llvm::ReturnInst* re = *ret.begin();
            auto&& retTerm = FN.Term->getValueTerm(re->getReturnValue());
            auto&& st = pass->getFunctionState(calledFun);
            res = FN.State->Chain(res, StateSlicer(FN, TermSet{retTerm}).transform(st));
            FN.Slot->incorporateFunction(inst->getParent()->getParent());
            res = FN.State->Chain(res, FN.Predicate->getEqualityPredicate(retArg, retTerm));
        }
        return res;
    }

    void getFunUsages(const llvm::Function& F, TermVec arguments, PredicateState::Ptr curRes, TermVec prevArgs, const llvm::CallInst* c) {
        auto&& sliced = FN.State->Basic();
        if (isFirstArg){
            sliced = funcState;
            isFirstArg = false;
        }
        else {
            sliced = getArgUsagesForFun(F, arguments);
        }

        // search CallPredicates and insert states from functions
        // getFunUsages dont work because of all users
        //
        if(c != nullptr){
            CallPredicate::Ptr p = pass->getCallInstructionPredicate(c);
            if (p != nullptr){
                if (sliced->equals(FN.State->Basic().get())) {
                    CallsSummariesExtraction<Pass> cse(FN, pass->getFunctionState(&F), c, p, pass);
                    auto&& trans = cse.transform(pass->getFunctionState(&F));
                    auto&& slice = StateSlicer(FN, TermSet(arguments.begin(), arguments.end())).transform(trans);
                    sliced = slice;
                }
                else {
                    CallsSummariesExtraction<Pass> cse(FN, sliced, c, p, pass);
                    auto&& trans = cse.transform(pass->getFunctionState(&F));
                    auto&& slice = StateSlicer(FN, TermSet(arguments.begin(), arguments.end())).transform(trans);
                    sliced = slice;
                }

            }
        }
        if (F.users().begin() == F.users().end()) {
            /*Replace args*/
            for(auto i = 0U; i < arguments.size(); ++i){
                auto&& sliced1 = ReplaceTermTransformer(FN, arguments[i], prevArgs[i]).transform(sliced);
                if(sliced1->equals(sliced.get()) && not(prevArgs[i]->equals(arguments[i].get()))){
                    auto&& eq = FN.Predicate->getEqualityPredicate(prevArgs[i], arguments[i]);
                    sliced = FN.State->Chain(FN.State->Basic(std::vector<Predicate::Ptr>{eq}), sliced);
                }
                else{
                    sliced = sliced1;
                }
            }


            curRes = FN.State->Chain(sliced, curRes);
            endPoint(curRes, F);
            return;
        }
        auto curFunSliced = sliced;
        auto curFunRes = curRes;
        for (auto&& it : F.users()) {
            if (auto&& call = llvm::dyn_cast<llvm::CallInst>(it)) {
                sliced = curFunSliced;
                curRes = curFunRes;
                auto&& fun = call->getParent()->getParent();
                TermVec newArgs;
                TermVec curFunArgs;
                for(auto&& arg : F.getArgumentList()){
                    auto&& argTerm = FN.Term->getValueTerm(&arg);
                    curFunArgs.push_back(argTerm);
                }
                int vec_ind = 0;
                for(auto&& i = 0U; i < curFunArgs.size(); ++i){
                    auto&& arg = call->getArgOperand(i);
                    auto&& tmp = FN.Term->getValueTerm(arg);
                    //if(auto&& call = llvm::dyn_cast<llvm::CallInst>(arg)){
                        //auto&& st = getStateIfArgIsFun(call, tmp);
                        //assert(st!=NULL);
                        //sliced = FN.State->Chain(st, sliced);
                    //}
                    if (isInterestingTerm(tmp)){
                        newArgs.push_back(tmp);
                        ++vec_ind;
                    }
                    else {
                        auto&& eq = FN.Predicate->getEqualityPredicate(curFunArgs[vec_ind], tmp);
                        sliced = FN.State->Chain(FN.State->Basic(std::vector<Predicate::Ptr>{eq}), sliced);
                        curFunArgs.erase(curFunArgs.begin() + vec_ind);
                    }
                }
                for(auto i = 0U; i < arguments.size(); ++i){
                    auto&& sliced1 = ReplaceTermTransformer(FN, arguments[i], prevArgs[i]).transform(sliced);
                    if(sliced1->equals(sliced.get()) && not(prevArgs[i]->equals(arguments[i].get()))){
                        auto&& eq = FN.Predicate->getEqualityPredicate(prevArgs[i], arguments[i]);
                        sliced = FN.State->Chain(FN.State->Basic(std::vector<Predicate::Ptr>{eq}), sliced);
                    }
                    else{
                        sliced = sliced1;
                    }

                }
                curRes = FN.State->Chain(sliced, curRes);
                getFunUsages(*fun, newArgs, curRes, curFunArgs, call);
            }
        }
        return;
    }


    PredicateState::Ptr getArgUsagesForFun(const llvm::Function& F, const TermVec args) {
        auto&& ps = pass->getFunctionState(&F);
        return StateSlicer(FN, TermSet(args.begin(), args.end())).transform(ps);
    }

    PredicateState::Ptr getArgUsagesForFun(PredicateState::Ptr state, const TermVec args) {
        return StateSlicer(FN, TermSet(args.begin(), args.end())).transform(state);
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


    FactoryNest FN;
    const llvm::Instruction* I;
    PredicateState::Ptr query;
    Pass* pass;
    PredicateState::Ptr resulting;

    std::vector<PredicateState::Ptr> resChoices;

    borealis::TypeFactory::Ptr TF;
    bool isFirstArg;
    PredicateState::Ptr funcState;

};

} /* namespace borealis */

#include "Util/unmacros.h"
#endif /* SUMMARIESEXTRACTOR_HPP_ */