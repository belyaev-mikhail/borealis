//
// Created by danya on 28.01.16.
//

#include "ReplaceTermTransformer.h"
#include <algorithm>
#include <string>

namespace borealis {

ReplaceTermTransformer::ReplaceTermTransformer(const FactoryNest& FN, TermSet from, const Term::Ptr to):
        Base(FN), replaceable(from), replacement(to) {
}

ReplaceTermTransformer::ReplaceTermTransformer(const FactoryNest& FN, Term::Ptr from, const Term::Ptr to):
        Base(FN), replacement(to) {
    replaceable.insert(from);
}

Term::Ptr ReplaceTermTransformer::transformTerm(Term::Ptr term) {
    if(util::contains(replaceable, term))
        return replacement;
    else
        return term;
}

}
