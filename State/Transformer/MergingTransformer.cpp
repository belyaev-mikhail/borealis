//
// Created by kivi on 21.10.15.
//

#include "SMT/Z3/Z3.h"
#include "SMT/Z3/ExprFactory.h"
#include "SMT/Z3/Solver.h"

#include "MergingTransformer.h"

namespace borealis {

MergingTransformer::MergingTransformer(const FactoryNest& fn, MemInfo& memInfo, int calls)
        : Base(fn), FN(fn), fMemInfo(memInfo), functionCalls(calls) {}

Predicate::Ptr MergingTransformer::transformPredicate(Predicate::Ptr pred) {
    ++contracts[pred];
    return pred;
}

PredicateState::Ptr MergingTransformer::getMergedState() {
    Z3::ExprFactory ef;
    Z3::Solver s(ef, fMemInfo.first, fMemInfo.second);

    std::vector<Predicate::Ptr> result;
    PredicateSet forDelete;

    for (auto&& it : contracts) {
        result.push_back(it.first);
    }

    for (auto i = 0U; i < result.size(); ++i) {
        for (auto j = i + 1; j < result.size(); ++j) {
            if (s.isPossible(result[i], result[j]).isSat()) {
                if (s.isStronger(result[i], result[j]).isUnsat()) {
                    forDelete.insert(result[j]);
                } else if (s.isStronger(result[j],result[i]).isUnsat()) {
                    forDelete.insert(result[i]);
                }
            } else {
                forDelete.insert(result[i]);
                forDelete.insert(result[j]);
            }
        }
    }

    for (auto i = 0U; i < result.size(); ++i) {
        if (util::contains(forDelete, result[i])) {
            result.erase(result.begin() + i);
            --i;
        }
    }

    return FN.State->Basic(result);
}

}   /* namespace borealis */