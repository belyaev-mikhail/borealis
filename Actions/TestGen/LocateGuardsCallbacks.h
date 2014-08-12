/*
 * LocateGuardsCallbacks.h
 *
 *  Created on: Aug 11, 2014
 *      Author: gagarski
 */

#ifndef LOCATEGUARDSCALLBACKS_H_
#define LOCATEGUARDSCALLBACKS_H_

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/PPCallbacks.h>
#include "TestGen/SourceLocations.h"
#include "Util/util.hpp"

#include <iostream>
namespace borealis {

class LocateGuardsCallbacks : public clang::PPCallbacks {
public:
    LocateGuardsCallbacks(GuardsLocations* locations, const std::string& moduleName,
        clang::SourceManager* sm, clang::Preprocessor& pp) :
        locations(locations), moduleName(moduleName), sm(sm), pp(pp) {};
    virtual void Ifndef(clang::SourceLocation Loc, const clang::Token &MacroNameTok);
    virtual void Endif(clang::SourceLocation Loc, clang::SourceLocation IfLoc);

private:
    GuardsLocations* locations;
    const std::string& moduleName;
    clang::SourceManager* sm;
    clang::Preprocessor& pp;

};

} // namespace borealis

#endif /* LOCATEGUARDSCALLBACKS_H_ */
