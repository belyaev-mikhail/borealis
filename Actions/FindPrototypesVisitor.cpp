/* 
 * File:   FindPrototypesVisitor.cpp
 * Author: maxim
 * 
 * Created on 25 Март 2014 г., 12:18
 */

#include <bits/hashtable.h>
    
#include "Actions/FindPrototypesVisitor.h"
#include "TestGen/PrototypesInfo.h"

namespace borealis {

bool FindPrototypesVisitor::VisitFunctionDecl(clang::FunctionDecl* s) {
    if (s->isFirstDeclaration()) {
        std::string fileName = sm->getFileEntryForID(sm->getFileID(s->getLocation()))->getName();
        if (fileName.back() == 'h') {
            prototypes->locations.emplace(s->getNameAsString(), fileName);
        }
    }
    return true;
}

} // namespace borealis
