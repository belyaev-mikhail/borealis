//
// Created by danya on 28.01.16.
//

#include "ReplaceTermTransformer.h"
#include <algorithm>
#include <string>

namespace borealis {

    ReplaceTermTransformer::ReplaceTermTransformer(const FactoryNest& FN,const std::string& funName) : Base(FN), fName(funName) {}

    Term::Ptr ReplaceTermTransformer::transformValueTerm(Term::Ptr term){
        std::string newName="$$";
        newName=newName+fName+"_"+term->getName();
        return FN.Term->getValueTerm(term->getType(),newName);
    }
}
