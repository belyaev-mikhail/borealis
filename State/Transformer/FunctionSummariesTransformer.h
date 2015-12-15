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
    using TermToArg = std::unordered_map<Term::Ptr, int, TermHash, TermEquals>;
    using ArgToTerms = std::unordered_map<int, TermSet>;
    using ChoiceInfo = std::vector<std::vector<Predicate::Ptr>>;
    using PrVector = std::vector<Predicate::Ptr>;


public:
    FunctionSummariesTransformer(const FactoryNest& FN, const TermMap& TM,
                const ChoiceInfo& ci, const Term::Ptr rvt);

    using Base::transform;
    PredicateState::Ptr transform(PredicateState::Ptr ps);
    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    const PrVector& getProtectedPredicates() const{
        return protStates;
    }

    const ArgToTerms& getArgToTermMapping() const {
        return argToTerms;
    }

    TermSet& getTermSet() {
        return TS;
    }

private:
    bool isDepFromArg(Term::Ptr term);
    bool isOpaqueTerm(Term::Ptr term);

private:

    TermSet arguments;
    TermMap mapping;
    TermSet TS;
    PrVector protStates;

    TermToArg termToArg;
    ArgToTerms argToTerms;

    ChoiceInfo choiceInfo;
    Term::Ptr rtv;
    int curPredi;

};

} /*namespace borealis*/
#endif /* STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_ */
