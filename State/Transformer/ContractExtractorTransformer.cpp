/*
 * ContractExtractorTransformer.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <llvm/IR/Instructions.h>

#include "ContractExtractorTransformer.h"

namespace borealis {

ContractExtractorTransformer::ContractExtractorTransformer(const FactoryNest& fn, llvm::CallInst& I, const TermMap& termMap) :
    Base(fn) {

    for (auto&& i = 0U; i < I.getNumOperands(); ++i) {
        auto&& arg = I.getArgOperand(i);
        auto&& term = FN.Term->getValueTerm(arg);

        if (isOpaqueTerm(term)) continue;
        termToArg[term] = i;
        args.insert(term);

        if (auto&& value = util::at(termMap, term)) {
            auto&& res = value.getUnsafe();

            if (isOpaqueTerm(res)) continue;
            termToArg[res] = i;
            args.insert(res);
        }
    }
}

PredicateState::Ptr ContractExtractorTransformer::transform(PredicateState::Ptr ps) {
    return Base::transform(ps)
        ->filter([](auto&& p) { return !!p; })
        ->simplify();
}

Predicate::Ptr ContractExtractorTransformer::transformPredicate(Predicate::Ptr pred) {
    if (pred->getType() == PredicateType::PATH) {
        TermMap termMap;
        for (auto&& op : pred->getOperands()) {
            if (checkTermForArgs(op)) {
                termMap[op] = op;
            }
        }

        if (not termMap.empty()) {
            return Predicate::Ptr{ pred->replaceOperands(termMap) };
        }
    }

    return nullptr;
}

bool ContractExtractorTransformer::checkTermForArgs(Term::Ptr term) {
    auto&& argFound = false;

    for (auto&& t : Term::getFullTermSet(term)) {
        if (util::contains(args, t)) {
            argToTerms[termToArg[t]].insert(t);
            argFound = true;
        }
    }

    return argFound;
}

bool ContractExtractorTransformer::isOpaqueTerm(Term::Ptr term) {
    return llvm::is_one_of<
        OpaqueBoolConstantTerm,
        OpaqueIntConstantTerm,
        OpaqueFloatingConstantTerm,
        OpaqueStringConstantTerm,
        OpaqueNullPtrTerm
    >(term);
}

}  /* namespace borealis */
