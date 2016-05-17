//
// Created by stepanov on 2/29/16.
//

#include "UnusedGlobalsDeleter.h"


namespace borealis {

    UnusedGlobalsDeleter::UnusedGlobalsDeleter(const FactoryNest& FN, const TermSet pr) : Base(FN), protTerm(pr) {}


    PredicateState::Ptr UnusedGlobalsDeleter::transform(PredicateState::Ptr ps) {
        return Base::transform(ps->reverse())
                ->filter([](auto&& p) { return !!p; })
                ->reverse()
                ->simplify();
    }

    Predicate::Ptr UnusedGlobalsDeleter::transformPredicate(Predicate::Ptr pred){
        for (auto&& lhv : util::viewContainer(pred->getOperands()).take(1)) {
            if(auto&& valueLhv=llvm::dyn_cast<ValueTerm>(lhv)){
                if(valueLhv->isGlobal()){
                    auto&& ee=Term::getFullTermSet(lhv);
                    if(util::hasIntersect(protTerm,ee)){
                        for (auto&& rhv : util::viewContainer(pred->getOperands()).drop(1)) {
                            protTerm.insert(rhv);
                        }
                    }
                    else return nullptr;
                }
            }
        }
        return pred;
    }

}