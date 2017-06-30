#ifndef BOREALIS_CALLSSUMMARIESEXTRACTION_HPP
#define BOREALIS_CALLSSUMMARIESEXTRACTION_HPP

#include "State/Transformer/RenameTermTransformer.h"
#include "State/Transformer/ReplaceTermTransformer.h"
#include "State/Transformer/Transformer.hpp"
#include "Util/collections.hpp"
#include "Util/macros.h"

#include <vector>

namespace borealis {



template <class Pass>
class CallsSummariesExtraction : public borealis::Transformer<CallsSummariesExtraction<Pass>> {

    using Base = Transformer<CallsSummariesExtraction<Pass>>;
    using TermSet = std::unordered_set<Term::Ptr, TermHash, TermEquals>;

public:

    CallsSummariesExtraction(FactoryNest& FN, PredicateState::Ptr sliced, const llvm::CallInst* c, CallPredicate::Ptr callPr, Pass* p):
            Base(FN), call(c), callPred(callPr), pass(p), FactoryNest(FN) {
        auto&& tc = TermCollector(FN);
        tc.transform(sliced);
        auto&& terms = tc.getTerms();
        for (auto t : terms)
            if (isInterestingTerm(t))
                wantedTerms.insert(t);
    }

    PredicateState::Ptr transformBasic(BasicPredicateState::Ptr ps) {
        auto&& bps = llvm::dyn_cast<BasicPredicateState>(ps);
        auto&& result = FactoryNest.State->Basic();
        std::vector<Predicate::Ptr> data;
        for (auto&& pr : bps->getData()){
            data.push_back(pr);
            //Get all call predicates
            if (auto call_pred = llvm::dyn_cast<CallPredicate>(pr)) {
                for (auto&& p: call_pred->getArgs()){
                    if (call->getCalledFunction()->getName() != call_pred->getFunctionName()->getName()
                        && (util::contains(wantedTerms, p)
                        || (call_pred->getResult() != nullptr && util::contains(wantedTerms, call_pred->getResult())))){
                        auto&& func = call->getParent()->getParent();
                        auto&& called =
                                util::viewContainer(*func)
                                        .flatten()
                                        .filter(LAM(it, llvm::is_one_of<llvm::CallInst, llvm::InvokeInst>(it)))
                                        .map(LAM(i, llvm::ImmutableCallSite(&i).getCalledFunction()))
                                        .filter(LAM(f, f != nullptr && f->getName() == call_pred->getFunctionName()->getName()))
                                        .filter()
                                        .toHashSet();
                        for (auto&& f : called) {
                            if (f->isDeclaration())
                                break;

                            // Insert state instead of call predicate
                            data.erase(data.end() - 1);
                            auto&& state = pass->getFunctionState(f);
                            TermSet args;

                            //Rename all terms but not args
                            for(auto&& arg : f->getArgumentList()){
                                auto&& oldArgTerm = FactoryNest.Term->getArgumentTerm(&arg);
                                args.insert(oldArgTerm);
                            }
                            if (call_pred->getResult() != nullptr) {
                                args.insert(call_pred->getResult());
                            }
                            auto&& renamed_state = RenameTermTransformer(FactoryNest, call_pred->getFunctionName()->getName(), args).transform(state);
                            state = renamed_state;

                            //Replace
                            auto&& oldArg = f->getArgumentList().begin();
                            int c = f->getArgumentList().size();
                            for(auto&& arg : call_pred->getArgs()){
                                if( --c < 0)
                                    break;
                                auto&& oldArgTerm = FactoryNest.Term->getArgumentTerm(oldArg);
                                state = ReplaceTermTransformer(FactoryNest, oldArgTerm, arg).transform(state);
                                oldArg = func->getArgumentList().getNext(oldArg);
                            }
                            // now rename return value if exists
                            // Fixme maybe create equalityPredicate?
                            if (call_pred->getResult() != nullptr) {
                                state = ReplaceTermTransformer(FactoryNest, call_pred->getResult(), true).transform(state);
                            }
                            state = FactoryNest.State->Chain(FactoryNest.State->Basic(data), state);
                            result = FactoryNest.State->Chain(result, state);
                            data.clear();
                            args.clear();
                            break;
                        }
                        break;
                    }
                }
            }
        }
        result = FactoryNest.State->Chain(result, FactoryNest.State->Basic(data));
        return result;
    }

private:


    bool isInterestingTerm(Term::Ptr term) {
        return not (llvm::is_one_of<
                OpaqueBoolConstantTerm,
                OpaqueIntConstantTerm,
                OpaqueFloatingConstantTerm,
                OpaqueStringConstantTerm,
                OpaqueNullPtrTerm
        >(term)) && (term->getNumSubterms() == 0);
    }


    TermSet wantedTerms;
    const llvm::CallInst* call;
    CallPredicate::Ptr callPred;
    Pass* pass;
    FactoryNest FactoryNest;
};

}

#include "Util/unmacros.h"



#endif // BOREALIS_CALLSSUMMARIESEXTRACTION_HPP