/*
            argToTerms[termToArg[t]].insert(t);
            return true;
 * ContractExtractorTransformer.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include "State/Transformer/ContractExtractorTransformer.h"

namespace borealis {

ContractExtractorTransformer::ContractExtractorTransformer(const FactoryNest& fn, llvm::CallInst& I, const TermMap& m) :
    Base(fn), mapping(m) {

    for (auto&& i = 0U; i < I.getNumOperands(); ++i) {
        auto&& arg = I.getArgOperand(i);
        auto&& term = FN.Term->getValueTerm(arg);

        if (isOpaqueTerm(term)) continue;
        termToArg[term] = i;
        args.insert(term);

        if (auto&& optRef = util::at(mapping, term)) {
            auto&& res = optRef.getUnsafe();

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
        TermMap m;
        for (auto&& op : pred->getOperands()) {
            if (checkTerm(op)) {
                m[op] = op;
            }
            if (auto&& optRef = util::at(mapping, op)) {
                auto&& res = optRef.getUnsafe();
                if (checkTerm(res)) {
                    m[op] = res;
                };
            }
        }

        if (not m.empty()) {
            return Predicate::Ptr{ pred->replaceOperands(m) };
        }
    }

    return nullptr;
}

bool ContractExtractorTransformer::checkTerm(Term::Ptr term) {
    auto&& flag = false;

    for (auto&& t : Term::getFullTermSet(term)) {
        if (util::contains(args, t)) {
            argToTerms[termToArg[t]].insert(t);
            flag = true;
        }
        if (auto&& optRef = util::at(mapping, t)) {
            auto&& res = optRef.getUnsafe();
            if (util::contains(args, res)) {
                argToTerms[termToArg[res]].insert(t);
                flag = true;
            }
        }
    }

    return flag;
}

bool ContractExtractorTransformer::isOpaqueTerm(Term::Ptr term) {
    if (llvm::is_one_of<
        OpaqueBoolConstantTerm,
        OpaqueIntConstantTerm,
        OpaqueFloatingConstantTerm,
        OpaqueStringConstantTerm,
        OpaqueNullPtrTerm
    >(term)) {
        return true;
    } else {
        return false;
    }
}

}  /* namespace borealis */
