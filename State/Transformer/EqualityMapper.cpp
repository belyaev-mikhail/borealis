/*
 * EqualityMapper.cpp
 *
 *  Created on: 25 мая 2015 г.
 *      Author: kivi
 */

#include "EqualityMapper.h"

namespace borealis {

    EqualityMapper::EqualityMapper(FactoryNest FN) : Base(FN) {}
    Term::Ptr prev;
    Term::Ptr prevRhv;
    Predicate::Ptr EqualityMapper::transformEqualityPredicate(EqualityPredicatePtr pred) {
       // errs()<<"pred="<<pred->getLhv()<<"="<<pred->getRhv()<<"\n";
        if(util::at(mapping,pred->getLhv()) && pred->getType() == PredicateType::STATE){
            usedTerms.insert(pred->getLhv());
        }
        TermMap replacement;
        if(prev && pred->getRhv()->equals(prev.get()) && not (util::contains(usedTerms,prev)) ){
           // errs()<<"Pred="<<pred->getRhv()<<" Prev="<<prev<<"\n";
           // errs()<<"Change "<<pred->getRhv()<<" to "<<prevRhv<<"\n";
            replacement[pred->getRhv()]=prevRhv;
        }
        else{
            if (util::at(mapping,pred->getLhv()) /*|| llvm::isa<ReturnValueTerm>(pred->getLhv())*/) {prev=pred->getLhv();return pred;}
            for (auto&& subterm : Term::getFullTermSet(pred->getRhv())) {
                if (auto&& value = util::at(mapping, subterm)) {
                    if(not (util::contains(usedTerms,subterm)))
                        replacement[subterm] = value.getUnsafe();
                }
            }
        }
        auto&& newRhv = Term::Ptr{ pred->getRhv()->replaceOperands(replacement) };
        auto&& replaced = FN.Predicate->getEqualityPredicate(pred->getLhv(), newRhv, pred->getLocation(), pred->getType());
        replaced = Predicate::Ptr{ replaced->replaceOperands(replacement) };

        if (auto&& newEq = llvm::dyn_cast<EqualityPredicate>(replaced)) {
            if (not isOpaqueTerm(newEq->getRhv())) {
                if (auto&& castTerm = llvm::dyn_cast<CastTerm>(newEq->getRhv())) {
                    mapping[newEq->getLhv()] = castTerm->getRhv();
                } else {
                    mapping[newEq->getLhv()] = newEq->getRhv();
                }
            }
            else usedTerms.insert(newEq->getLhv());
        }
        prev=pred->getLhv();
        prevRhv=pred->getRhv();
        return replaced;
    }

    Predicate::Ptr EqualityMapper::transformPredicate(Predicate::Ptr pred) {
        if (pred->getType() == PredicateType::PATH) {
            return Predicate::Ptr{ pred->replaceOperands(mapping) };
        }
        return pred;
    }

    const EqualityMapper::TermMap& EqualityMapper::getMappedValues() const {
        return mapping;
    }

    bool EqualityMapper::isOpaqueTerm(Term::Ptr term) {
        return llvm::is_one_of<
                OpaqueBoolConstantTerm,
                OpaqueIntConstantTerm,
                OpaqueFloatingConstantTerm,
                OpaqueStringConstantTerm,
                OpaqueNullPtrTerm
        >(term);
    }

}  /* namespace borealis */