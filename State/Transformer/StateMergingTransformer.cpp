//
// Created by kivi on 07.07.15.
//

#include "StateMergingTransformer.h"

namespace borealis {

StateMergingTransformer::StateMergingTransformer(const FactoryNest& fn) : Base(fn), FN(fn) {}

PredicateState::Ptr StateMergingTransformer::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();
}

Predicate::Ptr StateMergingTransformer::transformPredicate(Predicate::Ptr pred) {
    return pred;
}

}   /* namespace borealis */

