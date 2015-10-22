//
// Created by kivi on 21.10.15.
//

#ifndef BOREALIS_MERGINGTRANSFORMER_H
#define BOREALIS_MERGINGTRANSFORMER_H

#include "Transformer.hpp"

namespace borealis {

class MergingTransformer : public Transformer<MergingTransformer> {

    using Base = Transformer<MergingTransformer>;
    using PredicateCounter = std::unordered_map<Predicate::Ptr, int, PredicateHash, PredicateEquals>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;

public:

    MergingTransformer(const FactoryNest& fn, int calls);

    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    PredicateState::Ptr getMergedState(double minProbability);

private:

    FactoryNest FN;
    int functionCalls;
    PredicateCounter predicates;

};

}   /* namespace borealis */

#endif //BOREALIS_MERGINGTRANSFORMER_H
