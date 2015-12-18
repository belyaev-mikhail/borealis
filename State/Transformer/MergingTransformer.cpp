//
// Created by kivi on 21.10.15.
//

#include <Passes/Contract/ContractManager.h>
#include "SMT/Z3/Z3.h"
#include "SMT/Z3/ExprFactory.h"
#include "SMT/Z3/Solver.h"

#include "MergingTransformer.h"

namespace borealis {

MergingTransformer::MergingTransformer(const FactoryNest& fn, MemInfo& memInfo, unsigned int calls)
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

    for (auto&& i = result.begin(); i != result.end(); ++i) {
        for (auto&& j = i + 1; j != result.end() ; ++j) {
            if (s.isPossible(*i, *j).isSat()) {
                if (s.isStronger(*i, *j).isUnsat()) {
                    forDelete.insert(*j);
                    contracts[*i] += contracts[*j];
                } else if (s.isStronger(*j, *i).isUnsat()) {
                    forDelete.insert(*i);
                    contracts[*j] += contracts[*i];
                }
            } else {
                forDelete.insert(*i);
                forDelete.insert(*j);
            }
        }
    }

    for (auto&& i = result.begin(); i != result.end(); ++i) {
        if (util::contains(forDelete, *i)) {
            result.erase(i);
            --i;
        } else if ((double)contracts[*i] / functionCalls < ContractManager::mergingConstant) {
            result.erase(i);
            --i;
        }
    }

    return FN.State->Basic(result);
}

}   /* namespace borealis */