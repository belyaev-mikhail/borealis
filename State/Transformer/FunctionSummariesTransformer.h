/*
 * FunctionSummariesTransformer.h
 *
 *  Created on: 22 июня 2015 г.
 *      Author: danya
 */

#ifndef STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_
#define STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_

#include <unordered_set>
#include <stack>

#include "State/Transformer/Transformer.hpp"
#include "State/Transformer/StateSlicer.h"

#include "Util/algorithm.hpp"

namespace borealis {

class FunctionSummariesTransformer : public borealis::Transformer<FunctionSummariesTransformer> {

    using Base = borealis::Transformer<FunctionSummariesTransformer>;

    using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;
    using PredTermMap = std::unordered_map<Predicate::Ptr, Term::Ptr, PredicateHash, PredicateEquals>;
    using PrVector = std::vector<Predicate::Ptr>;
    using VecTermSet = std::vector<std::unordered_set<Term::Ptr,TermHash, TermEquals>>;


public:
    FunctionSummariesTransformer(const FactoryNest& FN, const TermSet rtvMap, const Term::Ptr rtv);

    using Base::transform;
    PredicateState::Ptr transform(PredicateState::Ptr ps);
    Predicate::Ptr transformStore(StorePredicatePtr pred);
    Predicate::Ptr transformEquality(EqualityPredicatePtr pred);


    PredicateState::Ptr transformChoice(PredicateStateChoicePtr ps);
    PredicateState::Ptr transformPredicateStateChoice(PredicateStateChoicePtr ps);
    PredicateState::Ptr transformImply(PredicateStateImplyPtr ps);

    const PrVector& getProtectedPredicates() const{
        return protPreds;
    }

    const VecTermSet& getTermSet() {
        return ter;
    }

    const PredTermMap& getProtPredMapping() const {
        return protPredMapping;
    }

    bool getIsImpl(){
        return isImplyHere;
    }

    PrVector getRtvValues(){
        return rtvValues;
    }
private:
    bool isOpaqueTerm(Term::Ptr term);

private:

    TermSet TS;
    VecTermSet ter;
    PrVector protPreds;
    PredTermMap protPredMapping;
    TermSet rtvEquiv;
    std::stack<Predicate::Ptr> prStack;
    bool isImplyHere;
    PrVector rtvValues;
    Term::Ptr rtv;

};

} /*namespace borealis*/
#endif /* STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_ */