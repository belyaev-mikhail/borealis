/*
 * FunctionSummariesTransformer.h
 *
 *  Created on: 22 июня 2015 г.
 *      Author: danya
 */

#ifndef STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_
#define STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_

#include <unordered_set>

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
    FunctionSummariesTransformer(const FactoryNest& FN, const Term::Ptr rtvMap);

    using Base::transform;
    PredicateState::Ptr transform(PredicateState::Ptr ps);
    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    const PrVector& getProtectedPredicates() const{
        return protPreds;
    }

    VecTermSet& getTermSet() {
        return ter;
    }

    const PredTermMap& getProtPredMapping() const {
        return protPredMapping;
    }

private:
    bool isOpaqueTerm(Term::Ptr term);

private:

    TermSet TS;
    VecTermSet ter;
    PrVector protPreds;
    PredTermMap protPredMapping;
    Predicate::Ptr curPathPr;
    Term::Ptr rtvMapping;
};

} /*namespace borealis*/
#endif /* STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_ */