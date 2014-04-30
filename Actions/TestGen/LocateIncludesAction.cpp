/*
 * LocateIncludesAction.cpp
 *
 *  Created on: Apr 24, 2014
 *      Author: gagarski
 */

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Preprocessor.h>

#include "Actions/TestGen/LocateIncludesAction.h"
#include "Actions/TestGen/LocateIncludesCallbacks.h"
#include "TestGen/SourceLocations.h"
#include "clang/Lex/Pragma.h"
namespace borealis {

void LocateIncludesAction::ExecuteAction() {
    getCompilerInstance().getPreprocessor().addPPCallbacks(
            // Preprocessor destructor deletes callbacks
            new LocateIncludesCallbacks(locations, &(getCompilerInstance().getSourceManager()),
                    getCompilerInstance().getPreprocessor())
    );
    PreprocessOnlyAction::ExecuteAction();

}
} // namespace borealis
