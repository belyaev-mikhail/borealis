//
// Created by danya on 28.01.16.
//

#include "ReplaceTermTransformer.h"

namespace borealis {

    ReplaceTermTransformer::ReplaceTermTransformer(const FactoryNest& FN,TermMap& chM) : Base(FN), chMap(chM) {}

    Term::Ptr ReplaceTermTransformer::transformValueTerm(Term::Ptr term){
        if(auto&& k = util::at(chMap,term)){
            term=k.getUnsafe();
        }
        return term;
    }
}
