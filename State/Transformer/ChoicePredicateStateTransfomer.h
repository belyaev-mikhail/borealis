//
// Created by danya on 01.10.15.
//

#ifndef BOREALIS_CHOICEPREDICATESTATETRANSFOMER_H
#define BOREALIS_CHOICEPREDICATESTATETRANSFOMER_H


#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <llvm/IR/Instructions.h>

#include "Transformer.hpp"

namespace borealis {

class ChoicePredicateStateTransfomer : public Transformer<ChoicePredicateStateTransfomer> {

    using Base = Transformer<ChoicePredicateStateTransfomer>;
    using ChoiceInfo = std::vector<std::vector<Predicate::Ptr>>;

public:

    ChoicePredicateStateTransfomer(const FactoryNest& fn);

    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    void pushBackTemp();
    ChoiceInfo getChoiceInfo() {return choiceInfo;}
    using Base::transformBase;

private:

    int curChoice;
    ChoiceInfo choiceInfo;
    std::vector <Predicate::Ptr> temp;

};

}



#endif //BOREALIS_CHOICEPREDICATESTATETRANSFOMER_H
