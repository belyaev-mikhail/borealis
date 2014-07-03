/* 
 * File:   FindPrototypesVisitor.cpp
 * Author: maxim
 * 
 * Created on 25 Март 2014 г., 12:18
 */

#include <clang/Basic/FileManager.h>
#include <llvm/Support/PathV2.h>

#include "Actions/FindPrototypesVisitor.h"
#include "TestGen/FunctionsInfoData.h"
#include "TestGen/Util/util.h"

namespace borealis {

bool FindPrototypesVisitor::VisitFunctionDecl(clang::FunctionDecl* s) {
    if (util::isSystem(s->getLocation(), *sm)) {
        return true;
    }
    if (s->isFirstDeclaration()) {
        auto* fileEntry = sm->getFileEntryForID(sm->getFileID(s->getLocation()));
        if (fileEntry != nullptr) {
            llvm::StringRef fileName = fileEntry->getName();
            if (llvm::sys::path::extension(fileName) == ".h") {
                fInfoData->locations.emplace(s->getNameAsString(), fileName);
            }
        }
    }
    return true;
}

} // namespace borealis
