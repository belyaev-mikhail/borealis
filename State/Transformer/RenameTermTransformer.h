//
// Created by stepanov on 6/15/17.
//

#ifndef BOREALIS_RENAMETERMTRANSFORMER_H
#define BOREALIS_RENAMETERMTRANSFORMER_H


#include <llvm/IR/Instructions.h>

#include "Transformer.hpp"

#include <string>

namespace borealis {

class RenameTermTransformer : public Transformer<RenameTermTransformer> {

    using Base = Transformer<RenameTermTransformer>;
    using TermSet = std::unordered_set<Term::Ptr, TermHash, TermEquals>;

public:

    RenameTermTransformer(const FactoryNest& FN, std::string name, TermSet arguments);

    Term::Ptr transformTerm(Term::Ptr term);

    bool isInterestingTerm(Term::Ptr term);

private:

    std::string new_name;
    TermSet args;
};


}


#endif //BOREALIS_RENAMETERMTRANSFORMER_H
