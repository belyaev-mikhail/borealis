/*
 * CallContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORMER_H_
#define STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORMER_H_

#include <unordered_set>

#include "State/Transformer/Transformer.hpp"

namespace borealis {

class ContractExtractorTransformer: public borealis::Transformer<ContractExtractorTransformer> {

    using Base = borealis::Transformer<ContractExtractorTransformer>;

    using TermSet = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;
    using TermToArg = std::unordered_map<Term::Ptr, int, TermHash, TermEquals>;
    using ArgToTerms = std::unordered_map<int, TermSet>;

public:

    ContractExtractorTransformer(const FactoryNest& fn, llvm::CallInst& I, const TermMap& m);

    PredicateState::Ptr transform(PredicateState::Ptr ps);
    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    const ArgToTerms& getArgToTermMapping() const {
        return argToTerms;
    }

private:

    bool checkTerm(Term::Ptr term);
    bool isOpaqueTerm(Term::Ptr term);

private:

    TermSet visited;

    TermSet args;
    TermMap mapping;

    TermToArg termToArg;
    ArgToTerms argToTerms;

};

}  /* namespace borealis */

#endif /* STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORMER_H_ */
