/*
 * ContractExtractorTransformer.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */
#include "State/Transformer/ContractExtractorTransformer.h"

namespace borealis {

ContractExtractorTransformer::ContractExtractorTransformer(const FactoryNest& fn, llvm::CallInst& I, Mapper& m) :
    Transformer(fn), args(), FN(fn), mapping(m), mapToInt(), mapToTerms() {
    int num = 0;
    for(auto&& it : I.arg_operands()) {
        auto&& term = FN.Term->getValueTerm(&*it);
        if(llvm::isa<OpaqueBoolConstantTerm>(term) || llvm::isa<OpaqueIntConstantTerm>(term)
                || llvm::isa<OpaqueFloatingConstantTerm>(term) || llvm::isa<OpaqueNullPtrTerm>(term)) continue;
        args.insert(term);
        mapToInt[term] = num;
        if(auto&& optRef = util::at(mapping, term)) {
            auto&& res = optRef.get();
            args.insert(*res);
            mapToInt[*res] = num;
        }
        ++num;
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
            if(checkTerm(i)) {
                return pred;
            }
            if(auto&& optRef = util::at(mapping, i)) {
                auto&& res = optRef.get();
                if(checkTerm(*res)) {
                    return FN.Predicate->getEqualityPredicate(*res, pred->getOperands()[pred->getNumOperands() - 1], Locus(), PredicateType::PATH);
                }
            }
        }
    }
    return nullptr;
}

bool ContractExtractorTransformer::checkTerm(Term::Ptr term) {
    for(auto&& it : Term::getFullTermSet(term)) {
        if(util::contains(args, it)) {
            mapToTerms[mapToInt[it]].insert(it);
            return true;
        }
        if(auto&& optRef = util::at(mapping, it)) {
            auto&& res = optRef.get();
            if(util::contains(args, *res)) {
                mapToTerms[mapToInt[*res]].insert(it);
                return true;
            }
        }
    }
    return false;
}

}  /* namespace borealis */



