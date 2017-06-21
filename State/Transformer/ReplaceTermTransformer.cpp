//
// Created by danya on 28.01.16.
//

#include "ReplaceTermTransformer.h"

namespace borealis {

ReplaceTermTransformer::ReplaceTermTransformer(const FactoryNest& FN, TermSet from, const Term::Ptr to):
        Base(FN), replaceable(from), replacement(to), isRTV(false) {
}

ReplaceTermTransformer::ReplaceTermTransformer(const FactoryNest& FN, Term::Ptr from, const Term::Ptr to):
        Base(FN), replacement(to), isRTV(false) {
    replaceable.insert(from);
}

ReplaceTermTransformer::ReplaceTermTransformer(const FactoryNest& FN, const Term::Ptr to, bool isReturnValue):
        Base(FN), replacement(to), isRTV(isReturnValue) { }

Term::Ptr ReplaceTermTransformer::transformTerm(Term::Ptr term) {
    for(auto&& it : replaceable){
        if(term->getName() == it->getName()){
            return replacement;
        }
    }
    return term;
}

Term::Ptr ReplaceTermTransformer::transformReturnValueTerm(ReturnValueTermPtr term){
    if (isRTV)
        return replacement;
    else
        return term;
}

}
