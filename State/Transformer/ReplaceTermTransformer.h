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

    public:

        ReplaceTermTransformer(const FactoryNest& fn,const std::string& funName);
        Term::Ptr transformValueTerm(Term::Ptr term);

    private:
        std::string fName;
};


}

#endif //BOREALIS_REPLACETERMTRANSFORMER_H
