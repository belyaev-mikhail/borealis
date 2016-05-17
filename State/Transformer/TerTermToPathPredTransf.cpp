//
// Created by stepanov on 5/16/16.
//

#include "TerTermToPathPredTransf.h"
#include <vector>

namespace borealis {

TerTermToPathPredTransf::TerTermToPathPredTransf(const FactoryNest &FN) : Base(FN) { }

PredicateState::Ptr TerTermToPathPredTransf::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
            ->filter([](auto&& p) { return !!p; })
            ->simplify();;

}

Term::Ptr TerTermToPathPredTransf::transformTernaryTerm(Term::Ptr term) {
    containTernTerm=true;
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
    newChoice = FN.State->Choice(std::vector<PredicateState::Ptr>{path,path2});
    return term;
}


Predicate::Ptr TerTermToPathPredTransf::transformEquality(EqualityPredicatePtr pred) {
    ++counterTT;
    curLhv=pred->getLhv();
    return Base::transformEquality(pred);
}

PredicateState::Ptr TerTermToPathPredTransf::transformBasic(BasicPredicateStatePtr ps){
    containTernTerm=false;
    counterTT=-1;
    return Base::transformBasic(ps);
}

PredicateState::Ptr TerTermToPathPredTransf::transformBasicPredicateState(BasicPredicateStatePtr ps){
    auto&& basic=llvm::dyn_cast<BasicPredicateState>(ps);
    if(containTernTerm){
        std::vector<Predicate::Ptr> base;
        for(int i=0; i<counterTT-1; ++i){
            base.push_back(basic->getData()[i]);
        }
        auto&& chain=FN.State->Chain(FN.State->Basic(base),newChoice);
        base.clear();
        for(int i=counterTT; i<ps->getData().size(); ++i){
            base.push_back(basic->getData()[i]);
        }
        auto&& basePointer = std::static_pointer_cast<const BasicPredicateState>(FN.State->Basic(base));
        auto&& endCh = transformBasic(basePointer);
        auto&& finalChain=FN.State->Chain(chain,endCh);
        return finalChain;
    }
    return ps;
}

}