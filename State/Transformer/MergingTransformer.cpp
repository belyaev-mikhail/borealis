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
    std::vector<Predicate::Ptr> state;

    for (auto&& it : contracts) {
        state.push_back(it.first);
    }

    if (state.empty()) return FN.State->Basic();

    deleteOppositePredicates(state);
    mergePredicates(state);

    return FN.State->Basic(state);
}

void MergingTransformer::deleteOppositePredicates(std::vector<Predicate::Ptr>& state) {
    Z3::ExprFactory ef;
    Z3::Solver s(ef, fMemInfo.first, fMemInfo.second);

    PredicateSet unsatCore;

    //simple unsat core extraction
    while(true) {
        auto&& uc = s.getUnsatCore(state);
        if (uc.empty()) break;

        std::vector<int> indexes;
        indexes.reserve(state.size());
        for (auto i = 0U; i < uc.size(); i++) {
            std::stringstream toString;
            std::stringstream toInt;
            int indx;

            toString << uc[i];
            toInt << toString.str().substr(1, toString.str().length() - 1);
            toInt >> indx;
            indexes.push_back(indx);

            unsatCore.insert(state[indx]);
        }

        std::sort(indexes.begin(), indexes.end(), std::greater<int>());
        for (auto&& indx : indexes) state.erase(state.begin() + indx);
    }

    //full unsat core extraction
    while (not state.empty()) {
        bool finished = true;
        for (auto&& pred : unsatCore) {
            auto&& newUC = s.getUnsatCore(state, pred);

            std::vector<int> indexes;
            indexes.reserve(state.size());
            for (auto i = 0U; i < newUC.size(); ++i) {
                std::stringstream toString;
                std::stringstream toInt;
                int indx;

                toString << newUC[i];
                if (toString.str() == "$$uc$$") continue;

                toInt << toString.str().substr(1, toString.str().length() - 1);
                toInt >> indx;
                indexes.push_back(indx);

                unsatCore.insert(state[indx]);
            }

            std::sort(indexes.begin(), indexes.end(), std::greater<int>());
            for (auto&& indx : indexes) state.erase(state.begin() + indx);

            if (not indexes.empty()) finished = false;
            if (state.empty()) break;
        }
        if (finished) break;
    }
}

void MergingTransformer::mergePredicates(std::vector<Predicate::Ptr> &state) {
    Z3::ExprFactory ef;
    Z3::Solver s(ef, fMemInfo.first, fMemInfo.second);
    PredicateSet forDelete;

    for (auto&& i = state.begin(); i != state.end(); ++i) {
        for (auto&& j = i + 1; j != state.end(); ++j) {
            if (s.isWeaker(*i, *j).isUnsat()) {
                forDelete.insert(*j);
                contracts[*i] += contracts[*j];
            } else if (s.isWeaker(*j, *i).isUnsat()) {
                forDelete.insert(*i);
                contracts[*j] += contracts[*i];
            }
        }
    }

    for (auto&& i = state.begin(); i != state.end(); ++i) {
        if (util::contains(forDelete, *i)) {
            state.erase(i);
            --i;
        } else if ((double)contracts[*i] / functionCalls < ContractManager::MERGING_CONSTANT) {
            state.erase(i);
            --i;
        }
    }
}

}   /* namespace borealis */