//
// Created by danya on 26.11.15.
//
#include <algorithm>

#include <llvm/IR/Instructions.h>

#include "UnnecesPredDeleter.h"
#include "ReplaceTermTransformer.h"


namespace borealis {

    UnnecesPredDeleter::UnnecesPredDeleter(const FactoryNest& FN,const PrVector& prPred,const TermSet& suspT,
                                           llvm::iplist<llvm::Argument>& ar):
            Base(FN), protPred(prPred),suspTerms(suspT),args(ar) {
            for (auto ar = args.begin(), er = args.end(); ar != er; ++ar) {
                llvm::Value *x = ar;
                auto &&term = FN.Term->getValueTerm(x);
                argTerms.insert(term);
            }
            isDel=true;
            inResVect=false;
        }


    PredicateState::Ptr UnnecesPredDeleter::transform(PredicateState::Ptr ps){
        return Base::transform(ps)
                ->filter([](auto&& p) { return !!p; })
                ->simplify();
    }


    Predicate::Ptr UnnecesPredDeleter::transformBase(Predicate::Ptr pr){
        auto&& eqpr=llvm::dyn_cast<EqualityPredicate>(pr);
        //LHV
        if(eqpr->getLhv()->getNumSubterms()==0){
            if(std::find(argTerms.begin(),argTerms.end(),eqpr->getLhv())==argTerms.end()){
                auto&& newTerm=FN.Term->getValueTerm(eqpr->getLhv()->getType(),"$$" + eqpr->getLhv()->getName());
                chMap.insert({eqpr->getLhv(),newTerm});
            }
        }
        Term::Ptr newLhv;
        if(eqpr->getLhv()->getNumSubterms()==0){
            auto&& it=chMap.find(eqpr->getLhv());
            if(it!=chMap.end()){
                newLhv=it->second;
            }
        }
        else {
            newLhv = ReplaceTermTransformer(FN,chMap).transform(eqpr->getLhv());
        }
        //RHV
        auto&& newRhv=Term::Ptr{eqpr->getRhv()->replaceOperands(chMap)};
        auto&& newPred=FN.Predicate->getEqualityPredicate(newLhv,newRhv,pr->getLocation(),pr->getType());
        if(!isDel){
            return newPred;
        }
        else{
            return nullptr;
        }

    }


    BasicPredicateState::Ptr UnnecesPredDeleter::transformBasic(BasicPredicateStatePtr pred) {
        intersTerms.clear();
        for (auto ar = args.begin(), er = args.end(); ar != er; ++ar) {
            llvm::Value *x = ar;
            auto &&term = FN.Term->getValueTerm(x);
            bool isArg = false;
            bool isInTerm = false;
            bool isProt = false;
            auto&& sec=pred->filterByTypes({PredicateType::PATH});
            if(pred->size()==sec->size()){
                if(!util::hasIntersect(pred->getData(),protPred)){
                    continue;
                }
            }
            for (auto &&k:pred->getData()) {
                if(util::contains(protPred,k))
                    isProt=true;
                if(pred->size()==1 && !isProt && pred->getData()[0]->getType()==PredicateType::PATH) break;
                for (auto &&n:k->getOperands()) {
                    if(isProt){
                        auto set=Term::getFullTermSet(n);
                        if(util::hasIntersect(set,temp) || util::contains(set,term)){
                            if(util::contains(newChoice,pred)==false){
                                isDel=false;
                                //addChoiceWithChTerms(pred);
                                inResVect=true;
                                if(temp.size()==0)temp.insert(term);
                                contractTerms.insert(n);
                            }
                            break;
                        }
                    }
                    auto&& operTermSet=Term::getFullTermSet(n);
                    util::getIntersect(suspTerms,operTermSet,std::inserter(intersTerms,intersTerms.end()));
                    if(intersTerms.size()!=0)isInTerm=true;
                    if(util::contains(operTermSet,term))
                    {
                        if(util::contains(newChoice,pred)==false){
                            isDel=false;
                            //addChoiceWithChTerms(pred);
                        }
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
                if(util::contains(newChoice,pred)==false) {
                    isDel=false;
                    //addChoiceWithChTerms(pred);
                }
                for(auto&& k:intersTerms) temp.insert(k);
            }
        }
        return Base::transformBasic(pred);
    }

    PredicateState::Ptr UnnecesPredDeleter::transformPredicateState(PredicateState::Ptr state){
        return state;
    }

    BasicPredicateState::Ptr UnnecesPredDeleter::transformBasicPredicateState(BasicPredicateStatePtr pred){
        isDel=true;
        if(pred->getData()[0]!=NULL){
            newChoice.push_back(pred);
        }
        else {
            return FN.State->Basic();
        }
        if(inResVect && newChoice.size()!=0){
            resultVec.push_back(FN.State->Choice(newChoice));
            inResVect=false;
        }
        return pred;
    }

    bool UnnecesPredDeleter::isOpaqueTerm(Term::Ptr term) {
        return llvm::is_one_of<
                OpaqueBoolConstantTerm,
                OpaqueIntConstantTerm,
                OpaqueFloatingConstantTerm,
                OpaqueStringConstantTerm,
                OpaqueNullPtrTerm
        >(term);
    }

}  /* namespace borealis */
