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

        TerTermToPathPredTransf(const FactoryNest& fn);

        PredicateState::Ptr transform(PredicateState::Ptr ps);
        PredicateState::Ptr transformBasic(BasicPredicateStatePtr ps);
        PredicateState::Ptr transformBasicPredicateState(BasicPredicateStatePtr ps);

        Predicate::Ptr transformEquality(EqualityPredicatePtr pred);
        Predicate::Ptr transformStore(StorePredicatePtr pred);
        Predicate::Ptr transformPredicate(Predicate::Ptr pred);

        Term::Ptr transformTernaryTerm(Term::Ptr term);

    private:
        Term::Ptr curLhv;
        std::vector<PredicateState::Ptr> newChoices;
        bool containTernTerm;
        int counterTT;
        std::vector<int> countersTT;
};

}

#endif //BOREALIS_TERTERMTOPATHPREDTRANSF_H

