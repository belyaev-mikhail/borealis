//
// Created by danya on 01.10.15.
//

#include "ChoicePredicateStateTransfomer.h"

namespace borealis {

    ChoicePredicateStateTransfomer::ChoicePredicateStateTransfomer(const FactoryNest& FN):Base(FN){ }


    int curChoice=0;
    std::vector <Predicate::Ptr> temp;

    Predicate::Ptr ChoicePredicateStateTransfomer::transformPredicate(Predicate::Ptr pred){
        if(pred->getType()==PredicateType::PATH) {
            if(curChoice!=0)
                pushBackTemp();
            ++curChoice;
            temp.clear();
            temp.push_back(pred);
        }
        else temp.push_back(pred);


        return pred;
    }


    void ChoicePredicateStateTransfomer::pushBackTemp() {
       choiceInfo.push_back(temp);
    }
}