/*
 * LocateIncludesCallbacks.cpp
 *
 *  Created on: Apr 24, 2014
 *      Author: gagarski
 */

#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/PPCallbacks.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/Token.h>

#include "Actions/TestGen/LocateIncludesCallbacks.h"
#include "TestGen/SourceLocations.h"
#include "Util/util.hpp"
#include <iostream>
namespace borealis {
void LocateIncludesCallbacks::InclusionDirective(clang::SourceLocation hashLoc,
                                  const clang::Token &includeToc,
                                  clang::StringRef fileName,
                                  bool isAngled,
                                  const clang::FileEntry *file,
                                  clang::SourceLocation endLoc,
                                  llvm::StringRef searchPath,
                                  llvm::StringRef relativePath) {

    util::use(includeToc);
    util::use(isAngled);
    util::use(file);
    util::use(searchPath);
    util::use(relativePath);
    util::use(endLoc);
    (*locations)[sm->getFileEntryForID(sm->getFileID(hashLoc))->getName()].insert(
            {fileName.str(), sm->getFileOffset(hashLoc), isAngled}
    );

}
} // namespace borealis
