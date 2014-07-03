/*
 * LocateIncludesCallbacks.h
 *
 *  Created on: Apr 23, 2014
 *      Author: gagarski
 */

#ifndef LOCATEINCLUDESCALLBACKS_H_
#define LOCATEINCLUDESCALLBACKS_H_

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/PPCallbacks.h>
#include "TestGen/SourceLocations.h"
#include "Util/util.hpp"

#include <iostream>
namespace borealis {

class LocateIncludesCallbacks : public clang::PPCallbacks {
public:
    LocateIncludesCallbacks(IncludesLocations* locations, clang::SourceManager* sm, clang::Preprocessor& pp) :
        locations(locations), sm(sm), pp(pp) {};
    virtual void InclusionDirective(clang::SourceLocation hashLoc,
                                      const clang::Token &includeToc,
                                      clang::StringRef fileName,
                                      bool isAngled,
                                      const clang::FileEntry *file,
                                      clang::SourceLocation endLoc,
                                      llvm::StringRef searchPath,
                                      llvm::StringRef relativePath);
private:
    IncludesLocations* locations;
    clang::SourceManager* sm;
    clang::Preprocessor& pp;

};
} // namespace borealis



#endif /* LOCATEINCLUDESCALLBACKS_H_ */
