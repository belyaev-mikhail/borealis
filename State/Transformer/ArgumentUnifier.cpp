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
            auto&& newTerm = FN.Term->getValueTerm(type, ARGUMENT_PREFIX + std::to_string(it.first));
            args.insert(newTerm);
            return newTerm;
        }
    }
    return term;
}

Term::Ptr ArgumentUnifier::transformCmpTerm(CmpTermPtr term) {
    if (isSigned(term->getOpcode())) {
        TermMap replacement;
        for (auto&& subterm : term->getSubterms()) {
            auto&& intTerm = llvm::dyn_cast<OpaqueIntConstantTerm>(subterm);
            auto&& type = llvm::dyn_cast<borealis::type::Integer>(subterm->getType());
            if (intTerm && type) {
                long long maxVal = 1;
                maxVal <<= type->getBitsize();
                if (intTerm->getValue() > ((maxVal / 2) - 1)) {
                    --maxVal;
                    replacement[subterm] = FN.Term->getIntTerm(-(maxVal - intTerm->getValue() + 1), term->getType());
                }
            }
        }
        return Term::Ptr{ term->replaceOperands(replacement) };
    }
    return term;
}

bool ArgumentUnifier::isSigned(llvm::ConditionType cond) {
    using CondType = llvm::ConditionType;
    if (cond != CondType::ULT && cond != CondType::ULE && cond != CondType::UGE && cond != CondType::UGT)
        return true;
    return false;
}

const ArgumentUnifier::TermSet& ArgumentUnifier::getArguments() {
    return args;
}

}   /* namespace borealis */