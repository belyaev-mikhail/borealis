//
// Created by danya on 26.11.15.
//
#include <algorithm>

#include <llvm/IR/Instructions.h>

#include "StateRipper.h"
#include "ReplaceTermTransformer.h"


namespace borealis {

    StateRipper::StateRipper(const FactoryNest& FN,const Predicate::Ptr& prPred):
            Base(FN), protPred(prPred) {
            contProt=false;
        }


    PredicateState::Ptr StateRipper::transform(PredicateState::Ptr ps){
        return Base::transform(ps)
                ->filter([](auto&& p) { return !!p; })
                ->simplify();
    }


    Predicate::Ptr StateRipper::transformBase(Predicate::Ptr pr){
        if(pr->equals(protPred.get())){
            contProt=true;
        }
        return pr;
    }


    BasicPredicateState::Ptr StateRipper::transformBasic(BasicPredicateStatePtr pred) {
        if(!contProt)
            return Base::transformBasic(pred);
        else
            return pred;
    }


    PredicateState::Ptr StateRipper::transformChoice(PredicateStateChoicePtr state){
        for(auto&& i=0U;i<state->getChoices().size();++i){
            auto&& newState=Base::transform(state->getChoices()[i]);
            if(contProt){
                return newState;
            }

        }
        return state;
    }

    PredicateState::Ptr StateRipper::transformChain(PredicateStateChainPtr state){
        if(!contProt){
            auto&& in=Base::transform(state->getBase());
            auto&& in2=Base::transform(state->getCurr());
            if(contProt){
                return FN.State->Chain(in,in2);
            }
            return state;
        }
        else {
            return state;
        }
    }

}  /* namespace borealis */
