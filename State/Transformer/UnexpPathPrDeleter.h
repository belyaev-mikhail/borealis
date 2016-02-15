//
// Created by stepanov on 2/4/16.
//

#ifndef BOREALIS_UNEXPPATHPRDELETER_H
#define BOREALIS_UNEXPPATHPRDELETER_H


#include <unordered_set>
#include <vector>

#include "State/Transformer/Transformer.hpp"

namespace borealis {

    class UnexpPathPrDeleter : public borealis::Transformer<UnexpPathPrDeleter> {

        using Base = Transformer<UnexpPathPrDeleter>;

    public:
        UnexpPathPrDeleter(const FactoryNest& FN, const Predicate::Ptr& prPred);

        using Base::transform;

        PredicateState::Ptr transform(PredicateState::Ptr ps);
        BasicPredicateState::Ptr transformBasic(BasicPredicateStatePtr pred);
        BasicPredicateState::Ptr transformBasicPredicateState(BasicPredicateStatePtr pred);
        Predicate::Ptr transformBase(Predicate::Ptr pr);

        using Base::transformBase;

    private:

        Predicate::Ptr protPred;
        bool isOnlyPath;
        bool isProt;
    };

} /*namespace borealis*/

#endif //BOREALIS_UNEXPPATHPRDELETER_H


