/*
 * FunctionSummariesTransformer.cpp
 *
 *  Created on: 22 июня 2015 г.
 *      Author: danya
 */

#include "FunctionSummariesTransformer.h"
#include <vector>

namespace borealis{

FunctionSummariesTransformer::FunctionSummariesTransformer(const FactoryNest& FN, const TermSet rtvMap, const Term::Ptr rtv) :
        Base(FN), rtvEquiv(rtvMap), isImplyHere(false), rtv(rtv) { }

PredicateState::Ptr FunctionSummariesTransformer::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();

}


PredicateState::Ptr FunctionSummariesTransformer::transformChoice(PredicateStateChoicePtr ps){
    prStack.push(nullptr);
    return Base::transformChoice(ps);
}

PredicateState::Ptr FunctionSummariesTransformer::transformImply(PredicateStateImplyPtr ps){
    isImplyHere=true;
    return ps;
}

PredicateState::Ptr FunctionSummariesTransformer::transformPredicateStateChoice(PredicateStateChoicePtr ps){
    prStack.pop();
    return ps;
}

Predicate::Ptr FunctionSummariesTransformer::transformStore(StorePredicatePtr pred){
    if(pred->getType() == PredicateType::PATH){
        if(prStack.size()!=0){
            prStack.pop();
        }
        prStack.push(pred);
    }
    else {
        if(rtvEquiv.find(pred->getLhv()) == rtvEquiv.end()) return pred;
        if(not isOpaqueTerm(pred->getRhv())) return pred;
        if(prStack.size()==0) return pred;
        if(prStack.top()==nullptr) return pred;
        for (auto &&op : prStack.top()->getOperands()) {
            for (auto&& t : Term::getFullTermSet(op)) {
                if(!isOpaqueTerm(t)&&t->getNumSubterms()==0){
                    TS.insert(t);
                    rtvValues.push_back(FN.Predicate->getStorePredicate(rtv, pred->getRhv()));
                    if(!util::contains(protPreds,prStack.top()))
                        protPreds.push_back(prStack.top());
                    protPredMapping.insert(std::make_pair(prStack.top(),pred->getRhv()));
                }
            }
            if(TS.size()!=0)
                ter.push_back(TS);
            TS.clear();
        }
    }
    return pred;
}

Predicate::Ptr FunctionSummariesTransformer::transformEquality(EqualityPredicatePtr pred){
    if(pred->getType() == PredicateType::PATH){
        if(prStack.size()!=0){
            prStack.pop();
        }
        prStack.push(pred);
    }
    else {
        if(rtvEquiv.find(pred->getLhv())==rtvEquiv.end()) return pred;
        if(prStack.size()==0) return pred;
        if(prStack.top()==nullptr) return pred;
        auto&& val = FN.Predicate->getEqualityPredicate(rtv, pred->getRhv());
        for(auto&& it : rtvValues){
            if(it->equals(val.get()))
                return pred;
        }
        rtvValues.push_back(FN.Predicate->getEqualityPredicate(rtv, pred->getRhv()));
        if(not isOpaqueTerm(pred->getRhv())) return pred;
        for (auto &&op : prStack.top()->getOperands()) {
            for (auto&& t : Term::getFullTermSet(op)) {
                if(not isOpaqueTerm(t)&&t->getNumSubterms()==0){
                    TS.insert(t);
                    if(!util::contains(protPreds,prStack.top()))
                        protPreds.push_back(prStack.top());
                    protPredMapping.insert(std::make_pair(prStack.top(),pred->getRhv()));
                    /*if(!util::contains(protPreds,prStack.top())){
                        TS.insert(t);
                        protPreds.push_back(prStack.top());
                        protPredMapping.insert(std::make_pair(prStack.top(),eqPred->getRhv()));
                    }*/
                }
            }
            if(TS.size()!=0)
                ter.push_back(TS);
            TS.clear();
        }
    }
    return pred;
}


bool FunctionSummariesTransformer::isOpaqueTerm(Term::Ptr term) {
    return llvm::is_one_of<
            OpaqueBoolConstantTerm,
            OpaqueIntConstantTerm,
            OpaqueFloatingConstantTerm,
            OpaqueStringConstantTerm,
            OpaqueNullPtrTerm
    >(term);
}

} /*namespace borealis*/