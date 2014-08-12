/*
 * LocateGuardsCallbacks.cpp
 *
 *  Created on: Aug 11, 2014
 *      Author: gagarski
 */

#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/PPCallbacks.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/Token.h>

#include "Actions/TestGen/LocateGuardsCallbacks.h"
#include "Passes/TestGeneration/TestDumpPass.h"
#include "TestGen/SourceLocations.h"
#include "Util/util.hpp"
#include <iostream>

namespace borealis {

void LocateGuardsCallbacks::Ifndef(clang::SourceLocation Loc, const clang::Token &MacroNameTok) {
    if (MacroNameTok.getIdentifierInfo() != nullptr &&
            MacroNameTok.getIdentifierInfo()->getName() ==
            TestDumpPass::getOracleHeaderIncludeGuard(moduleName)) {
        (*locations)[sm->getFileEntryForID(sm->getFileID(Loc))->getName()].ifndef =
                sm->getFileOffset(Loc);
    }
}

void LocateGuardsCallbacks::Endif(clang::SourceLocation Loc, clang::SourceLocation IfLoc) {
    if (sm->getFileOffset(IfLoc) ==
            (*locations)[sm->getFileEntryForID(sm->getFileID(IfLoc))->getName()].ifndef) {
        (*locations)[sm->getFileEntryForID(sm->getFileID(Loc))->getName()].endif =
                sm->getFileOffset(Loc);
        (*locations)[sm->getFileEntryForID(sm->getFileID(Loc))->getName()].found = true;
    }
}

} // namespace borealis
