/*
 * LocateFunctionsVisitor.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: gagarski
 */

#include "Actions/TestGen/LocateFunctionsVisitor.h"

namespace borealis {

bool LocateFunctionsVisitor::VisitFunctionDecl(clang::FunctionDecl* s) {
    if (sm->getFileEntryForID(sm->getFileID(s->getLocation())) != nullptr) { // WTF ???
        (*locations)[sm->getFileEntryForID(sm->getFileID(s->getLocation()))->getName()].insert({
            s->getNameAsString(),
            sm->getFileOffset(s->getTypeSpecStartLoc()),
            sm->getFileOffset(s->getLocation()),
            sm->getFileOffset(s->getLocEnd()),
            s->getBody() != nullptr ? sm->getFileOffset(s->getBody()->getLocStart()) : 0,
            s->getBody() != nullptr ? sm->getFileOffset(s->getBody()->getLocEnd()) : 0
        });
    }

    return true;
}
} // namespace borealis
