//
// Created by stepanov on 5/17/16.
//

#include "FindRTVEquiv.h"

namespace borealis {

FindRTVEquiv::FindRTVEquiv(const FactoryNest& FN, Term::Ptr rt) : Base(FN) {rtvEquiv.insert(rt);}

PredicateState::Ptr FindRTVEquiv::transform(PredicateState::Ptr ps){
    auto reversed = ps->reverse();
    return Base::transform(reversed)
            ->filter([](auto&& p) { return !!p; })
            ->reverse()
            ->simplify();
}

Predicate::Ptr FindRTVEquiv::transformEqualityPredicate(EqualityPredicatePtr pred){
    if(rtvEquiv.find(pred->getLhv())!=rtvEquiv.end()){
        if(pred->getRhv()->getNumSubterms()==0 && !isOpaqueTerm(pred->getRhv())){
            rtvEquiv.insert(pred->getRhv());
        }
    }
    return pred;
}

Predicate::Ptr FindRTVEquiv::transformStorePredicate(StorePredicatePtr pred){
    if(rtvEquiv.find(pred->getLhv()) != rtvEquiv.end() && storeTerms.find(pred->getLhv()) == storeTerms.end()){
        if(pred->getRhv()->getNumSubterms()==0 && !isOpaqueTerm(pred->getRhv())){
            storeTerms.insert(pred->getLhv());
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
