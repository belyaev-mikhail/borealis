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
class FunctionSummariesTransformer: public borealis::Transformer<FunctionSummariesTransformer> {

    using Base = borealis::Transformer<FunctionSummariesTransformer>;

    using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;
    using TermToArg = std::unordered_map<Term::Ptr, int, TermHash, TermEquals>;
    using ArgToTerms = std::unordered_map<int, TermSet>;
    using ChoiceInfo = std::vector<std::vector<Predicate::Ptr>>;


public:
    FunctionSummariesTransformer(const FactoryNest& FN, llvm::iplist<llvm::Argument>& args,const TermMap& TM,
    const ChoiceInfo& ci,const Term::Ptr rvt);

    using Base::transform;
    PredicateState::Ptr transform(PredicateState::Ptr ps);
    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    const ArgToTerms& getArgToTermMapping() const {
        return argToTerms;
    }

private:

    bool checkTerm(Term::Ptr term);
    bool isOpaqueTerm(Term::Ptr term);

private:


    TermSet arguments;
    TermMap mapping;

    TermToArg termToArg;
    ArgToTerms argToTerms;

    ChoiceInfo choiceInfo;
    Term::Ptr rtv;
};

} /*namespace borealis*/
#endif /* STATE_TRANSFORMER_FUNCTIONSUMMARIESTRANSFORMER_H_ */
