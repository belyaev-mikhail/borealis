//
// Created by stepanov on 2/4/16.
//


#include <algorithm>

#include "UnexpPathPrDeleter.h"
#include "ReplaceTermTransformer.h"

#include <llvm/IR/Instructions.h>

namespace borealis {

    UnexpPathPrDeleter::UnexpPathPrDeleter(const FactoryNest& FN, const Predicate::Ptr& prPred): Base(FN),protPred(prPred) { }


    PredicateState::Ptr UnexpPathPrDeleter::transform(PredicateState::Ptr ps){
        return Base::transform(ps)
                ->filter([](auto&& p) { return !!p; })
                ->simplify();
    }


    Predicate::Ptr UnexpPathPrDeleter::transformBase(Predicate::Ptr pr){
        if(pr->getType()!=PredicateType::PATH)
            isOnlyPath=false;
        if(pr->equals(protPred.get()))
            isProt=true;
        return pr;
    }


    BasicPredicateState::Ptr UnexpPathPrDeleter::transformBasic(BasicPredicateStatePtr pred) {
        isOnlyPath=true;
        isProt=false;
        return Base::transformBasic(pred);
    }


    BasicPredicateState::Ptr UnexpPathPrDeleter::transformBasicPredicateState(BasicPredicateStatePtr pred){
        if(!isOnlyPath || isProt)
            return pred;
        else
            return FN.State->Basic();
    }

}  /* namespace borealis */
