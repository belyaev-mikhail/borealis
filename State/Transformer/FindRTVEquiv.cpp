//
// Created by stepanov on 5/17/16.
//

#include "FindRTVEquiv.h"

namespace borealis {

FindRTVEquiv::FindRTVEquiv(const FactoryNest& FN, Term::Ptr rt) : Base(FN) {rtvEquiv.insert(rt);}

Predicate::Ptr FindRTVEquiv::transformEqualityPredicate(EqualityPredicatePtr pred){
    if(rtvEquiv.find(pred->getLhv())!=rtvEquiv.end()){
        if(pred->getRhv()->getNumSubterms()==0 && !isOpaqueTerm(pred->getRhv())){
            rtvEquiv.insert(pred->getRhv());
        }
    }
    return pred;
}


bool FindRTVEquiv::isOpaqueTerm(Term::Ptr term) {
    return llvm::is_one_of<
            OpaqueBoolConstantTerm,
            OpaqueIntConstantTerm,
            OpaqueFloatingConstantTerm,
            OpaqueStringConstantTerm,
            OpaqueNullPtrTerm
    >(term);
}

}
