//
// Created by kivi on 21.10.15.
//

#ifndef BOREALIS_MERGINGTRANSFORMER_H
#define BOREALIS_MERGINGTRANSFORMER_H

#include "Transformer.hpp"

namespace borealis {

class MergingTransformer : public Transformer<MergingTransformer> {

    using Base = Transformer<MergingTransformer>;
    using PredicateCounter = std::unordered_map<Predicate::Ptr, unsigned int, PredicateHash, PredicateEquals>;
    using PredicateSet = std::unordered_set<Predicate::Ptr, PredicateHash, PredicateEquals>;
    using MemInfo = std::pair<unsigned int, unsigned int>;

public:

    MergingTransformer(const FactoryNest& fn, MemInfo& memInfo, unsigned int calls);

    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    PredicateState::Ptr getMergedState();

private:

    void deleteOppositePredicates(std::vector<Predicate::Ptr>& state);
    void mergePredicates(std::vector<Predicate::Ptr>& state);

private:

    FactoryNest FN;
    MemInfo fMemInfo;
    unsigned int functionCalls;
    PredicateCounter contracts;

};

}   /* namespace borealis */

#endif //BOREALIS_MERGINGTRANSFORMER_H
