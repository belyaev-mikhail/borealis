//
// Created by kivi on 21.10.15.
//

#include "SMT/Z3/Z3.h"
#include "SMT/Z3/ExprFactory.h"
#include "SMT/Z3/Solver.h"

#include "MergingTransformer.h"

namespace borealis {

MergingTransformer::MergingTransformer(const FactoryNest& fn, MemInfo& memInfo, TermSet args, int calls)
        : Base(fn), FN(fn), fMemInfo(memInfo), arguments(args), functionCalls(calls) {}

Predicate::Ptr MergingTransformer::transformPredicate(Predicate::Ptr pred) {
    for (auto&& arg : arguments) {
        if (containsTerm(pred, arg)) {
            ++contractsByArg[arg][pred];
        }
    }
    return pred;
}

PredicateState::Ptr MergingTransformer::getMergedState() {
    Z3::ExprFactory ef;
    Z3::Solver s(ef, fMemInfo.first, fMemInfo.second);

    std::vector<Predicate::Ptr> result;

    for (auto&& it : contractsByArg) {
        std::vector<Predicate::Ptr> currMerge;
        PredicateSet forDelete;
        for (auto&& predCounter : it.second) {
            if (currMerge.empty()) currMerge.push_back(predCounter.first);
            else {
                for (auto&& pred : currMerge) {
                    if (pred == nullptr) continue;
                    auto&& res = s.getStrongerPredicate(predCounter.first, pred);
                    if (res == nullptr) forDelete.insert(pred);
                    else if (res->equals(predCounter.first.get())) {
                        forDelete.insert(pred);
                        currMerge.push_back(predCounter.first);
                    }
                }
            }
        }
        for (auto&& pred : currMerge) {
            if (not util::contains(forDelete, pred))
                result.push_back(pred);
        }
    }

    return FN.State->Basic(result);
}

bool MergingTransformer::containsTerm(Predicate::Ptr predicate, Term::Ptr term) {
    for (auto&& op : predicate->getOperands()) {
        if (util::contains(Term::getFullTermSet(op), term)) {
            return true;
        }
    }
    return false;
}

}   /* namespace borealis */