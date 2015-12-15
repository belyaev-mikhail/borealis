//
// Created by danya on 22.09.15.
//

#include <llvm/IR/Instructions.h>

#include "ChoiceInfoCollector.h"

namespace borealis {

    ChoiceInfoCollector::ChoiceInfoCollector(const FactoryNest& FN) : Base(FN) {curChoice=0;choiceCounter=0;}

    PredicateState::Ptr ChoiceInfoCollector::transformChoice(PredicateStateChoicePtr pred) {
        ++curChoice;
        ++choiceCounter;
        return Base::transformChoice(pred);
    }

    Predicate::Ptr ChoiceInfoCollector::transformPredicate(Predicate::Ptr pred){
        return pred;
    }

    Predicate::Ptr ChoiceInfoCollector::transformBase(Predicate::Ptr pred){
        if(curChoice!=0) {
            if (pred->getType() == PredicateType::PATH) {
                if (curChoice != 0) {
                    pushBackTemp();
                }
                temp.clear();
                temp.push_back(pred);
            }
            else temp.push_back(pred);
        }
        return pred;
    }

    PredicateState::Ptr ChoiceInfoCollector::transformPredicateStateChoice(PredicateStateChoicePtr pred){
        --curChoice;
        if(pred->getChoices().size()==choiceCounter) {
            pushBackTemp();
        }
        return pred;
    }

    void ChoiceInfoCollector::pushBackTemp() {
        if(temp.size()!=0)
        choiceInfo.push_back(temp);
    }
}  /* namespace borealis */
