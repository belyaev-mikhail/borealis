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

namespace borealis {

class FunctionSummariesTransformer : public borealis::Transformer<FunctionSummariesTransformer> {

    using Base = borealis::Transformer<FunctionSummariesTransformer>;

    using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;
    using PredTermMap = std::unordered_map<Predicate::Ptr, Term::Ptr, PredicateHash, PredicateEquals>;
    using ChoiceInfo = std::vector<std::vector<Predicate::Ptr>>;
    using PrVector = std::vector<Predicate::Ptr>;
    using VecTermSet = std::vector<std::unordered_set<Term::Ptr,TermHash, TermEquals>>;


public:
    FunctionSummariesTransformer(const FactoryNest& FN, const TermMap& TM,
                const ChoiceInfo& ci, const Term::Ptr rvt);

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
    bool isDepFromArg(Term::Ptr term);
    bool isOpaqueTerm(Term::Ptr term);

private:

    TermMap mapping;
    TermSet TS;
    VecTermSet ter;
    PrVector protPreds;

    ChoiceInfo choiceInfo;
    Term::Ptr rtv;
    int curPredi;
    PredTermMap protPredMapping;
};

} /*namespace borealis*/
#endif /* STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_ */
