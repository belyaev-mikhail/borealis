//
// Created by kivi on 06.07.15.
//

#include "ArgumentUnifier.h"

namespace borealis {

ArgumentUnifier::ArgumentUnifier(const FactoryNest& fn, const ArgToTerms& a)
        : Base(fn), FN(fn), argsMapping(a) {
}

Term::Ptr ArgumentUnifier::transformTerm(Term::Ptr term) {
    for (auto&& it : argsMapping) {
        if (util::contains(it.second, term)) {
            auto&& type = term->getType();
            auto&& newTerm = FN.Term->getArgumentTermExternal(it.first, ARGUMENT_PREFIX + std::to_string(it.first), type);
            args.insert(newTerm);
            return newTerm;
        }
    }
    return term;
}

bool ArgumentUnifier::isSigned(llvm::ConditionType cond) {
    using CondType = llvm::ConditionType;
    return cond != CondType::ULT && cond != CondType::ULE && cond != CondType::UGE && cond != CondType::UGT;
}

const ArgumentUnifier::TermSet& ArgumentUnifier::getArguments() {
    return args;
}

}   /* namespace borealis */