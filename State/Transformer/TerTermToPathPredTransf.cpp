//
// Created by stepanov on 5/16/16.
//

#include "TerTermToPathPredTransf.h"

#include <vector>

namespace borealis {

TerTermToPathPredTransf::TerTermToPathPredTransf(const FactoryNest &FN) : Base(FN) { }

PredicateState::Ptr TerTermToPathPredTransf::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; });

}

Term::Ptr TerTermToPathPredTransf::transformTernaryTerm(Term::Ptr term) {
    containTernTerm=true;
    countersTT.push_back(counterTT);
    auto&& pred1 = FN.Predicate->getEqualityPredicate(term->getSubterms()[0], FN.Term->getOpaqueConstantTerm(true),
                                                      Locus(), PredicateType::PATH);
    auto&& pred2 = FN.Predicate->getEqualityPredicate(curLhv, term->getSubterms()[1],
                                                      Locus(), PredicateType::STATE);
    auto&& path = FN.State->Basic(std::vector<Predicate::Ptr>{pred1,pred2});

    pred1 = FN.Predicate->getEqualityPredicate(term->getSubterms()[0], FN.Term->getOpaqueConstantTerm(false),
                                                      Locus(), PredicateType::PATH);
    pred2 = FN.Predicate->getEqualityPredicate(curLhv, term->getSubterms()[2],
                                                      Locus(), PredicateType::STATE);
    auto&& path2 = FN.State->Basic(std::vector<Predicate::Ptr>{pred1,pred2});
    newChoices.push_back(FN.State->Choice(std::vector<PredicateState::Ptr>{path,path2}));
    return term;
}


Predicate::Ptr TerTermToPathPredTransf::transformPredicate(Predicate::Ptr pred) {
    ++counterTT;
    return Base::transformPredicate(pred);
}

Predicate::Ptr TerTermToPathPredTransf::transformEquality(EqualityPredicatePtr pred){
    curLhv=pred->getLhv();
    return Base::transformEquality(pred);
}

Predicate::Ptr TerTermToPathPredTransf::transformStore(StorePredicatePtr pred) {
    curLhv=pred->getLhv();
    return Base::transformStore(pred);
}

PredicateState::Ptr TerTermToPathPredTransf::transformBasic(BasicPredicateStatePtr ps){
    containTernTerm=false;
    countersTT.clear();
    newChoices.clear();
    counterTT=-1;
    return Base::transformBasic(ps);
}

PredicateState::Ptr TerTermToPathPredTransf::transformBasicPredicateState(BasicPredicateStatePtr ps) {
    auto&& basic = llvm::dyn_cast<BasicPredicateState>(ps);
    if (containTernTerm) {
        std::vector<Predicate::Ptr> base;
        for (int i = 0; i <= countersTT[0]; ++i) {
            base.push_back(basic->getData()[i]);
        }
        auto&& finChain = FN.State->Chain(FN.State->Basic(base), newChoices[0]);
        if (countersTT.size() != 1){
            for(size_t i = 1; i<countersTT.size(); ++i){
                std::vector<Predicate::Ptr> base;
                for (int j = countersTT[i-1] + 2; j < countersTT[i] + 1; ++j) {
                    base.push_back(basic->getData()[j]);
                }
                finChain = FN.State->Chain(finChain, FN.State->Basic(base));
                finChain = FN.State->Chain(finChain, newChoices[i]);
            }
        }
        base.clear();
        for (size_t i = countersTT[countersTT.size()-1] + 2; i < ps->size(); ++i){
            base.push_back(basic->getData()[i]);
        }
        finChain = FN.State->Chain(finChain, FN.State->Basic(base));
        return finChain;
    }
    return ps;
}

}