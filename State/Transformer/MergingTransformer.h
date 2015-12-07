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
    using TermToPredicates = std::unordered_map<Term::Ptr, PredicateCounter, TermHash, TermEquals>;
    using TermSet = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using PredicateSet = std::unordered_set<Predicate::Ptr, PredicateHash, PredicateEquals>;
    using MemInfo = std::pair<unsigned int, unsigned int>;

public:

    MergingTransformer(const FactoryNest& fn, MemInfo& memInfo, TermSet args, int calls);

    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    PredicateState::Ptr getMergedState();

private:

    bool containsTerm(Predicate::Ptr predicate, Term::Ptr term);

private:

    FactoryNest FN;
    MemInfo fMemInfo;
    TermSet arguments;
    int functionCalls;
    TermToPredicates contractsByArg;

};

}   /* namespace borealis */

#endif //BOREALIS_MERGINGTRANSFORMER_H
