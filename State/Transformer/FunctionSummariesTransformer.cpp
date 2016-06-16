/*
 * FunctionSummariesTransformer.cpp
 *
 *  Created on: 22 июня 2015 г.
 *      Author: danya
 */

#include "FunctionSummariesTransformer.h"
#include <vector>

namespace borealis{

FunctionSummariesTransformer::FunctionSummariesTransformer(const FactoryNest& FN, const TermSet rtvMap) :
        Base(FN), rtvEquiv(rtvMap) {}

PredicateState::Ptr FunctionSummariesTransformer::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();;

}


PredicateState::Ptr FunctionSummariesTransformer::transformChoice(PredicateStateChoicePtr ps){
    Predicate::Ptr emptyPred;
    prStack.push(emptyPred);
    return Base::transformChoice(ps);
}


PredicateState::Ptr FunctionSummariesTransformer::transformPredicateStateChoice(PredicateStateChoicePtr ps){
    prStack.pop();
    return ps;
}


Predicate::Ptr FunctionSummariesTransformer::transformPredicate(Predicate::Ptr pred) {
    if(pred->getType()==PredicateType::PATH){
        if(prStack.size()!=0){
            prStack.pop();
        }
        prStack.push(pred);
        return pred;
    }
    else{
        auto&& eqPred=llvm::dyn_cast<EqualityPredicate>(pred);
        if(eqPred==NULL) return pred;
        if(rtvEquiv.find(eqPred->getLhv())==rtvEquiv.end()) return pred;
        if(not isOpaqueTerm(eqPred->getRhv())) return pred;
        for (auto &&op : prStack.top()->getOperands()) {
            for (auto&& t : Term::getFullTermSet(op)) {
                if(!isOpaqueTerm(t)&&t->getNumSubterms()==0){
                    TS.insert(t);
                    if(!util::contains(protPreds,prStack.top()))
                        protPreds.push_back(prStack.top());
                    protPredMapping.insert(std::make_pair(prStack.top(),eqPred->getRhv()));
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