/* 
 * File:   FindPrototypesVisitor.cpp
 * Author: maxim
 * 
 * Created on 25 Март 2014 г., 12:18
 */

#include <llvm/Support/PathV2.h>

#include "Actions/FindPrototypesVisitor.h"
#include "TestGen/PrototypesInfo.h"

namespace borealis {

bool FindPrototypesVisitor::VisitFunctionDecl(clang::FunctionDecl* s) {
    if (s->isFirstDeclaration()) {
        auto* fileEntry = sm->getFileEntryForID(sm->getFileID(s->getLocation()));
        if (fileEntry != nullptr) {
            llvm::StringRef fileName = fileEntry->getName();
            if (llvm::sys::path::extension(fileName) == ".h") {
                prototypes->locations.emplace(s->getNameAsString(), fileName);
            }
        }
    }
    return true;
}

} // namespace borealis
