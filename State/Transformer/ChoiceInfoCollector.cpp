//
// Created by danya on 22.09.15.
//

#include <llvm/IR/Instructions.h>

#include "ChoiceInfoCollector.h"
#include "ChoicePredicateStateTransfomer.h"

namespace borealis {

    ChoiceInfoCollector::ChoiceInfoCollector(const FactoryNest& FN) : Base(FN) {}

    PredicateState::Ptr ChoiceInfoCollector::transformChoice(PredicateStateChoicePtr pred) {
        auto&& chp = ChoicePredicateStateTransfomer(FN);
        chp.transform(pred.get()->self());
        chp.pushBackTemp();
        choiceInfo = chp.getChoiceInfo();
        return pred;
    }

}  /* namespace borealis */
