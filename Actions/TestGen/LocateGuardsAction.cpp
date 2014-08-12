/*
 * LocateGuardsAction.cpp
 *
 *  Created on: Aug 11, 2014
 *      Author: gagarski
 */

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Preprocessor.h>

#include "Actions/TestGen/LocateGuardsAction.h"
#include "Actions/TestGen/LocateGuardsCallbacks.h"
#include "TestGen/SourceLocations.h"
#include "clang/Lex/Pragma.h"
namespace borealis {

void LocateGuardsAction::ExecuteAction() {
    getCompilerInstance().getPreprocessor().addPPCallbacks(
            // Preprocessor destructor deletes callbacks
            new LocateGuardsCallbacks(locations, moduleName,
                    &(getCompilerInstance().getSourceManager()),
                    getCompilerInstance().getPreprocessor())
    );
    PreprocessOnlyAction::ExecuteAction();

}
} // namespace borealis
