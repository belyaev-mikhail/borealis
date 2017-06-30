//
// Created by stepanov on 6/15/17.
//

#include "RenameTermTransformer.h"

namespace borealis {

RenameTermTransformer::RenameTermTransformer(const FactoryNest& FN, std::string name, TermSet arguments):
        Base(FN), new_name(name), args(arguments) {
}


Term::Ptr RenameTermTransformer::transformTerm(Term::Ptr term) {
    if(isInterestingTerm(term) && !util::contains(args, term)){
        return FN.Term->getValueTerm(term->getType(), new_name + "_" + term->getName());
    }
    return term;
}

bool RenameTermTransformer::isInterestingTerm(Term::Ptr term) {
    return not (llvm::is_one_of<
            OpaqueBigIntConstantTerm,
            OpaqueBoolConstantTerm,
            OpaqueIntConstantTerm,
            OpaqueFloatingConstantTerm,
            OpaqueStringConstantTerm,
            OpaqueNamedConstantTerm,
            OpaqueNullPtrTerm,
            ReturnValueTerm
    >(term)) && (term->getNumSubterms() == 0);
}

}