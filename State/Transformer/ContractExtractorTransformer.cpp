/*
 * ContractExtractorTransformer.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */
#include "State/Transformer/ContractExtractorTransformer.h"

namespace borealis {

ContractExtractorTransformer::ContractExtractorTransformer(const FactoryNest& fn, llvm::CallInst& I, Mapper& m) :
    Transformer(fn),
    FN(fn),
    mapping(m) {
    for(auto&& it : I.arg_operands()) {
        auto&& term = FN.Term->getValueTerm(&*it);
        args.insert(term);
        if(auto&& optRef = util::at(mapping, term)) {
            auto&& res = optRef.get();
            args.insert(*res);
        }
    }
}

PredicateState::Ptr ContractExtractorTransformer::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
        ->filter([](auto&& p) { return !!p; })
        ->simplify();
}

Predicate::Ptr ContractExtractorTransformer::transformPredicate(Predicate::Ptr pred) {
    if(pred->getType() == PredicateType::PATH) {
        for(auto&& i : pred->getOperands()) {
            if(checkTerm(i)) return pred;
            if(auto&& optRef = util::at(mapping, i)) {
                auto&& res = optRef.get();
                if(checkTerm(*res)) return FN.Predicate->getEqualityPredicate(i, *res, Locus(), PredicateType::PATH);
            }
        }
    }
    return nullptr;
}

bool ContractExtractorTransformer::checkTerm(Term::Ptr term) {
    for(auto&& it : Term::getFullTermSet(term)) {
        if(util::contains(args, it)) {
            return true;
        }
        if(auto&& optRef = util::at(mapping, it)) {
            auto&& res = optRef.get();
            if(util::contains(args, *res)) {
                return true;
            }
        }
    }
    return false;
}

}  /* namespace borealis */



