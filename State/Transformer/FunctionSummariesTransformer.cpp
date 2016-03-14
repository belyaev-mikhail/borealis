/*
 * FunctionSummariesTransformer.cpp
 *
 *  Created on: 22 июня 2015 г.
 *      Author: danya
 */

#include "FunctionSummariesTransformer.h"
#include "State/Transformer/StateSlicer.h"
#include "State/Transformer/ChoiceInfoCollector.h"

#include "Util/algorithm.hpp"

namespace borealis{

    FunctionSummariesTransformer::FunctionSummariesTransformer(const FactoryNest& FN,
                                                               const TermMap& TM,
                                                               const ChoiceInfo& ci,const Term::Ptr rv
    ) :
            Base(FN), mapping(TM), choiceInfo(ci), rtv(rv), curPredi(-1) { }

    int count=0;
    PredicateState::Ptr FunctionSummariesTransformer::transform(PredicateState::Ptr ps) {
        return Base::transform(ps)
                ->filter([](auto&& p) { return !!p; })
                ->simplify();;

    }


    Predicate::Ptr FunctionSummariesTransformer::transformPredicate(Predicate::Ptr pred) {
        if (pred->getType() == PredicateType::PATH) {
            ++count;
            int k=choiceInfo.size();
            if (curPredi < k) {
                ++curPredi;
                if (choiceInfo[curPredi].size() >= 2) {
                    int isEq=0;
                    auto&& pred2=llvm::dyn_cast<EqualityPredicate>(choiceInfo[curPredi][choiceInfo[curPredi].size()-1]);
                    auto&& pred3=llvm::dyn_cast<StorePredicate>(choiceInfo[curPredi][choiceInfo[curPredi].size()-1]);
                    if(pred2!=NULL){
                        isEq=1;
                    }
                    if(pred3!=NULL){
                        isEq=2;
                    }
                    if(isEq==1){
                        if(not(pred2->getLhv()->equals(mapping[rtv].get()))){
                            return nullptr;}
                        if(not isOpaqueTerm(pred2->getRhv())) {
                            return nullptr;
                        }
                    }
                    else if(isEq==2){
                        if(not(pred3->getLhv()->equals(mapping[rtv].get()))){
                            return nullptr;}
                        if(not isOpaqueTerm(pred3->getRhv())) {
                            return nullptr;
                        }
                    }
                    else return pred;
                    for (auto &&op : pred->getOperands()) {
                        for (auto&& t : Term::getFullTermSet(op)) {
                            if(!isOpaqueTerm(t)&&t->getNumSubterms()==0){
                                TS.insert(t);
                                if(!util::contains(protPreds,pred))
                                    protPreds.push_back(pred);
                                if(isEq==1){
                                    protPredMapping.insert(std::make_pair(pred,pred2->getRhv()));
                                }
                                else{
                                    protPredMapping.insert(std::make_pair(pred,pred3->getRhv()));
                                }
                            }
                        }
                        if(TS.size()!=0)
                            ter.push_back(TS);
                        TS.clear();
                    }
                }
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
