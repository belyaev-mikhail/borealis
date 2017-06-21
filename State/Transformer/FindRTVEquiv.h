//
// Created by stepanov on 5/17/16.
//

#ifndef BOREALIS_FINDRTVEQUIV_H
#define BOREALIS_FINDRTVEQUIV_H


#include <unordered_set>
#include <stack>

#include "State/Transformer/Transformer.hpp"
#include "State/Transformer/StateSlicer.h"

#include "Util/algorithm.hpp"

namespace borealis{

class FindRTVEquiv: public Transformer<FindRTVEquiv> {

    using Base = Transformer<FindRTVEquiv>;
    using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;

public:

    FindRTVEquiv(const FactoryNest& fn, const Term::Ptr rt);

    PredicateState::Ptr transform(PredicateState::Ptr ps);
    Predicate::Ptr transformEqualityPredicate(EqualityPredicatePtr pred);
    Predicate::Ptr transformStorePredicate(StorePredicatePtr pred);

    bool isOpaqueTerm(Term::Ptr term);

    TermSet getRtvEquiv(){return rtvEquiv;}

private:
    TermSet rtvEquiv;
    TermSet storeTerms;

};


}



#endif //BOREALIS_FINDRTVEQUIV_H
