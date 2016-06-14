//
// Created by danya on 26.11.15.
//

#ifndef BOREALIS_UNNECESPREDDELETER_H
#define BOREALIS_UNNECESPREDDELETER_H


#include <unordered_set>
#include <vector>

#include "State/Transformer/Transformer.hpp"

namespace borealis {

    class StateRipper : public borealis::Transformer<StateRipper> {

        using Base = Transformer<StateRipper>;

    public:
        StateRipper(const FactoryNest& FN,const Predicate::Ptr& prPred);

        using Base::transform;

        PredicateState::Ptr transform(PredicateState::Ptr ps);
        BasicPredicateState::Ptr transformBasic(BasicPredicateStatePtr pred);
        PredicateState::Ptr transformChoice(PredicateStateChoicePtr state);
        PredicateState::Ptr transformChain(PredicateStateChainPtr state);
        Predicate::Ptr transformBase(Predicate::Ptr pr);

        using Base::transformBase;

        bool isOpaqueTerm(Term::Ptr term);
    private:

        Predicate::Ptr protPred;
        bool contProt;
        bool isFromBase;
    };

} /*namespace borealis*/
#endif //BOREALIS_UNNECESPREDDELETER_H
