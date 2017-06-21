//
// Created by danya on 28.01.16.
//

#ifndef BOREALIS_REPLACETERMTRANSFORMER_H
#define BOREALIS_REPLACETERMTRANSFORMER_H


#include <llvm/IR/Instructions.h>

#include "Transformer.hpp"

namespace borealis {

class ReplaceTermTransformer : public Transformer<ReplaceTermTransformer> {

using Base = Transformer<ReplaceTermTransformer>;
using TermSet = std::unordered_set<Term::Ptr, TermHash, TermEquals>;

public:

    ReplaceTermTransformer(const FactoryNest& FN, TermSet from, const Term::Ptr to);
    ReplaceTermTransformer(const FactoryNest& FN, Term::Ptr from, const Term::Ptr to);
    ReplaceTermTransformer(const FactoryNest& FN, const Term::Ptr to, bool isReturnValue = false);

    Term::Ptr transformTerm(Term::Ptr term);
    Term::Ptr transformReturnValueTerm(ReturnValueTermPtr term);


private:

    TermSet replaceable;
    const Term::Ptr replacement;
    bool isRTV;
};


}

#endif //BOREALIS_REPLACETERMTRANSFORMER_H
