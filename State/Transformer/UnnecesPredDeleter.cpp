//
// Created by danya on 26.11.15.
//
#include <algorithm>

#include <llvm/IR/Instructions.h>

#include "UnnecesPredDeleter.h"


namespace borealis {

    UnnecesPredDeleter::UnnecesPredDeleter(const FactoryNest& FN,const PrVector& prPred,const TermSet& suspT,
                                           llvm::iplist<llvm::Argument>& ar):
            Base(FN), protPred(prPred),suspTerms(suspT),args(ar) { }


    PredicateState::Ptr UnnecesPredDeleter::transform(PredicateState::Ptr ps){
        return Base::transform(ps)
                ->filter([](auto&& p) { return !!p; })
                ->simplify();
    }

    PredicateState::Ptr UnnecesPredDeleter::transformChoice(PredicateStateChoicePtr pred) {
        intersTerms.clear();
        StVector newChoice;
        for (auto ar = args.begin(), er = args.end(); ar != er; ++ar) {
            llvm::Value *x = ar;
            auto &&term = FN.Term->getValueTerm(x);
            for (auto&& i = 0U; i < pred->getChoices().size(); ++i) {
                bool isArg = false;
                bool isInTerm = false;
                bool isProt = false;
                auto&& fir = llvm::dyn_cast<BasicPredicateState>(pred->getChoices()[i]);
                auto&& sec=fir->filterByTypes({PredicateType::PATH});
                if(fir->size()==sec->size()){
                    if(!util::hasIntersect(fir->getData(),protPred)){
                        continue;
                    }
                }
                for (auto &&k:fir->getData()) {
                    if(util::contains(protPred,k))
                        isProt=true;
                    if(fir->size()==1 && !isProt) break;
                    for (auto &&n:k->getOperands()) {
                        if(isProt){
                            auto set=Term::getFullTermSet(n);
                            if(util::hasIntersect(set,contractTerms) || util::contains(set,term)){
                                if(util::contains(newChoice,pred->getChoices()[i])==false){
                                    newChoice.push_back(pred->getChoices()[i]);
                                }
                                break;
                            }
                        }
                        auto&& operTermSet=Term::getFullTermSet(n);
                        util::getIntersect(suspTerms,operTermSet,std::inserter(intersTerms,intersTerms.end()));
                        if(intersTerms.size()!=0)isInTerm=true;
                        if(util::contains(operTermSet,term))
                        {
                            if(util::contains(newChoice,pred->getChoices()[i])==false){
                                newChoice.push_back(pred->getChoices()[i]);}
                            if(k->getType()!=PredicateType::PATH){
                                auto pr=llvm::dyn_cast<EqualityPredicate>(k);
                                mayInfl.insert(pr->getLhv());
                            }
                            isArg=true;
                        }
                        if(util::hasIntersect(operTermSet,mayInfl) && k->getType()!=PredicateType::PATH) isArg=true;
                    }
                }
                if(isInTerm && isArg){
                    if(util::contains(newChoice,pred->getChoices()[i])==false)
                        newChoice.push_back(pred->getChoices()[i]);
                    for(auto&& k:intersTerms) contractTerms.insert(k);
                }
            }
        }
        return FN.State->Choice(newChoice);
    }




}  /* namespace borealis */
