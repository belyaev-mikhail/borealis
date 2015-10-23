//
// Created by kivi on 21.10.15.
//

#include "MergingTransformer.h"

namespace borealis {

MergingTransformer::MergingTransformer(const FactoryNest& fn, int calls) : Base(fn), FN(fn), functionCalls(calls) {}

Predicate::Ptr MergingTransformer::transformPredicate(Predicate::Ptr pred) {
    if (util::at(predicates, pred)) {
        ++predicates[pred];
    } else {
        predicates[pred] = 1;
    }
    return pred;
}

PredicateState::Ptr MergingTransformer::getMergedState(double minProbability) {
    TermMap booleanInvert;
    booleanInvert[FN.Term->getTrueTerm()] = FN.Term->getFalseTerm();
    booleanInvert[FN.Term->getFalseTerm()] = FN.Term->getTrueTerm();

    std::vector<Predicate::Ptr> result;

    for (auto&& it : predicates) {
        double probability = double(it.second)/double(functionCalls);
        auto&& pred = it.first;
        auto&& invertedPred = Predicate::Ptr{ pred->replaceOperands(booleanInvert) };
        if (util::at(predicates, invertedPred)) {
            if (it.second > predicates[invertedPred] && probability > 0.9) {
                result.push_back(pred);
            }
        } else if (probability > minProbability) {
            result.push_back(pred);
        }
    }

    return FN.State->Basic(result);
}

}   /* namespace borealis */