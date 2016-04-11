/*
 * FunctionSummariesTransformer.cpp
 *
 *  Created on: 22 июня 2015 г.
 *      Author: danya
 */

#include "FunctionSummariesTransformer.h"


namespace borealis{

FunctionSummariesTransformer::FunctionSummariesTransformer(const FactoryNest& FN, const Term::Ptr rtvMap) :
        Base(FN),rtvMapping(rtvMap) { }

PredicateState::Ptr FunctionSummariesTransformer::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();;

}


Predicate::Ptr FunctionSummariesTransformer::transformPredicate(Predicate::Ptr pred) {
    if(pred->getType()==PredicateType::PATH){
        curPathPr=pred;
        return pred;
    }
    else{
        auto&& eqPred=llvm::dyn_cast<EqualityPredicate>(pred);
        if(eqPred==NULL) return pred;
        if(not(eqPred->getLhv()->equals(rtvMapping.get()))) return pred;
        if(not isOpaqueTerm(eqPred->getRhv())) return pred;
        for (auto &&op : curPathPr->getOperands()) {
            for (auto&& t : Term::getFullTermSet(op)) {
                if(!isOpaqueTerm(t)&&t->getNumSubterms()==0){
                    TS.insert(t);
                    if(!util::contains(protPreds,curPathPr))
                        protPreds.push_back(curPathPr);
                    protPredMapping.insert(std::make_pair(curPathPr,eqPred->getRhv()));
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