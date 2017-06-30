
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

    SummariesExtractor(const FactoryNest& FN, const llvm::Instruction* i, PredicateState::Ptr q, Pass* pass, PredicateState::Ptr funState)
            : FN(FN), I(i), query(q), pass(pass), isFirstArg(true), funcState(funState) {
        TF = borealis::TypeFactory::get();
        auto&& fun = I->getParent()->getParent();
        if (fun->getArgumentList().size() == 0) {
            resulting = funState;
            return;
        }
        // Get and filter all terms from sliced state
        auto&& tc = TermCollector(FN);
        tc.transform(q);
        TermSet terms;
        for(auto&& it : tc.getTerms())
            if(isInterestingTerm(it))
                terms.insert(it);
        // Fixme
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
        //Simple decection of recursion
        std::vector<llvm::Function*> passedFunctions;
        // Fixme const?
        getFunUsages(*const_cast<llvm::Function*>(fun), argsNo, FN.State->Basic(), argsNo, nullptr, passedFunctions);
        //Fixme delete this if?
        if(resChoices.size()==1)
            resulting = resChoices[0];
        else
            resulting = FN.State->Choice(resChoices);
        //Fixme termcollector already exist
        tc = TermCollector(FN);
        tc.transform(resulting);
        // Replace all terms to terms from first function
        for(auto&& it : tc.getTerms()){
            if(llvm::isa<borealis::ArgumentTerm>(it)){
                auto&& term = FN.Term->getValueTerm(it->getType(), it->getName());
                resulting = ReplaceTermTransformer(FN, it, term).transform(resulting);
            }
        }
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

    void getFunUsages(llvm::Function& F, TermVec arguments, PredicateState::Ptr curRes, TermVec prevArgs,
                      const llvm::CallInst* c, std::vector<llvm::Function*> passedFunctions) {
        // Simple detection of recursion
        if (util::contains(passedFunctions, &F))
            return;
        passedFunctions.push_back(&F);
        auto&& sliced = FN.State->Basic();
        if (isFirstArg){
            sliced = funcState;
            isFirstArg = false;
        }
        else {
            sliced = getArgUsagesForFun(F, arguments);
            //Rename all terms but not args
            TermSet curArgs;
            for(auto&& arg : F.getArgumentList()){
                auto&& argTerm = FN.Term->getArgumentTerm(&arg);
                curArgs.insert(argTerm);
            }
            sliced = RenameTermTransformer(FN, F.getName(), curArgs).transform(sliced);
            //Rename arguments
            for(auto&& arg:arguments){
                if(!util::contains(curArgs, arg)){
                    arg = FN.Term->getValueTerm(arg->getType(), F.getName().str() + "_" + arg->getName());
                }
            }

        }

        // search CallPredicates and insert states from functions
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
                    auto&& trans = cse.transform(sliced);
                    auto&& slice = StateSlicer(FN, TermSet(arguments.begin(), arguments.end())).transform(trans);
                    sliced = slice;
                }

            }
        }
        if (F.users().begin() == F.users().end()) {
            curRes = FN.State->Chain(sliced, curRes);
            endPoint(curRes, F);
            return;
        }
        // Fixme
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
                // Fixme!
                if (curFunArgs.size() != call->getNumArgOperands())
                    continue;

                for(auto&& i = 0U; i < call->getNumArgOperands(); ++i){
                    FN.Slot->incorporateFunction(call->getParent()->getParent());
                    auto&& arg = call->getArgOperand(i);
                    auto&& tmp = FN.Term->getValueTerm(arg);
                    if (auto&& gep = llvm::dyn_cast<GepTerm>(tmp)){
                        newArgs.push_back(gep->getBase());
                        ++vec_ind;
                    }
                    else if (isInterestingTerm(tmp)){
                        auto&& retmp = FN.Term->getValueTerm(tmp->getType(), call->getParent()->getParent()->getName().str() + "_" + tmp->getName());
                        auto&& eq = FN.Predicate->getEqualityPredicate(curFunArgs[vec_ind], retmp);
                        sliced = FN.State->Chain(FN.State->Basic(std::vector<Predicate::Ptr>{eq}), sliced);
                        newArgs.push_back(tmp);
                        ++vec_ind;
                    }
                    else {
                        auto&& eq = FN.Predicate->getEqualityPredicate(curFunArgs[vec_ind], tmp);
                        sliced = FN.State->Chain(FN.State->Basic(std::vector<Predicate::Ptr>{eq}), sliced);
                        curFunArgs.erase(curFunArgs.begin() + vec_ind);
                    }
                }
                curRes = FN.State->Chain(sliced, curRes);
                getFunUsages(*fun, newArgs, curRes, curFunArgs, call, passedFunctions);
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
        return (not (llvm::is_one_of<
                OpaqueBoolConstantTerm,
                OpaqueIntConstantTerm,
                OpaqueFloatingConstantTerm,
                OpaqueStringConstantTerm,
                OpaqueNullPtrTerm
        >(term))) && (term->getNumSubterms() == 0);
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