/*
 * LocateGuardsAction.h
 *
 *  Created on: Aug 11, 2014
 *      Author: gagarski
 */

#include <clang/Frontend/FrontendActions.h>

#include "Actions/TestGen/LocateGuardsCallbacks.h"
#include "TestGen/SourceLocations.h"
#include "clang/Lex/Pragma.h"

#ifndef LOCATEGUARDSACTION_H_
#define LOCATEGUARDSACTION_H_

namespace borealis {
class LocateGuardsAction: public clang::PreprocessOnlyAction {
public:
    explicit LocateGuardsAction(GuardsLocations* locations, const std::string& moduleName) :
        locations(locations), moduleName(moduleName) {}

    void ExecuteAction();
private:
    GuardsLocations* locations;
    const std::string& moduleName;
};
}

#endif /* LOCATEGUARDSACTION_H_ */
