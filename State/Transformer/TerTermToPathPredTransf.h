//
// Created by stepanov on 5/16/16.
//

#ifndef BOREALIS_TERTERMTOPATHPREDTRANSF_H
#define BOREALIS_TERTERMTOPATHPREDTRANSF_H

#include <unordered_set>

#include "State/Transformer/Transformer.hpp"

#include "Util/algorithm.hpp"


namespace borealis {

class TerTermToPathPredTransf : public borealis::Transformer<TerTermToPathPredTransf> {

    using Base = Transformer<TerTermToPathPredTransf>;

    public:
        using Base::transform;
        PredicateState::Ptr transform(PredicateState::Ptr ps);
        TerTermToPathPredTransf(const FactoryNest& fn);
        Term::Ptr transformTernaryTerm(Term::Ptr term);
        Predicate::Ptr transformEquality(EqualityPredicatePtr pred);
        PredicateState::Ptr transformBasic(BasicPredicateStatePtr ps);
        PredicateState::Ptr transformBasicPredicateState(BasicPredicateStatePtr ps);

    private:
        Term::Ptr curLhv;
        PredicateState::Ptr newChoice;
        bool containTernTerm;
        int counterTT;
};

}

#endif //BOREALIS_TERTERMTOPATHPREDTRANSF_H

