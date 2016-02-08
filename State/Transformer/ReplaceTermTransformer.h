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
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;
    public:

        ReplaceTermTransformer(const FactoryNest& fn,TermMap& chM);

        Term::Ptr transformValueTerm(Term::Ptr term);
    private:

        TermMap chMap;
};


}

#endif //BOREALIS_REPLACETERMTRANSFORMER_H
