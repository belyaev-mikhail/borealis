/*
 * LocateIncludesAction.h
 *
 *  Created on: Apr 18, 2014
 *      Author: gagarski
 */
#include <clang/Frontend/FrontendActions.h>

#include "Actions/TestGen/LocateIncludesCallbacks.h"
#include "TestGen/SourceLocations.h"
#include "clang/Lex/Pragma.h"

#ifndef LOCATEINCLUDESACTION_H_
#define LOCATEINCLUDESACTION_H_

namespace borealis {
class LocateIncludesAction: public clang::PreprocessOnlyAction {
public:
    explicit LocateIncludesAction(IncludesLocations* locations) : locations(locations) {}

    void ExecuteAction();
private:
    IncludesLocations* locations;
};
}



#endif /* LOCATEINCLUDESACTION_H_ */
