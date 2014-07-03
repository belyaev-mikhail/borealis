/*
 * LocateFunctionsAction.h
 *
 *  Created on: Apr 17, 2014
 *      Author: gagarski
 */

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>

#include "Actions/TestGen/LocateFunctionsConsumer.h"
#include "TestGen/SourceLocations.h"
#include "Util/util.hpp"

#ifndef LOCATEFUNCTIONSACTION_H_
#define LOCATEFUNCTIONSACTION_H_

namespace borealis {

class LocateFunctionsAction: public clang::ASTFrontendAction {
public:
    explicit LocateFunctionsAction(FunctionsLocations* locations) : locations(locations) {}

    virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance& compiler,
                                                  llvm::StringRef inFile) {
        return new LocateFunctionsConsumer(&compiler.getASTContext(),
                &compiler.getSourceManager(), inFile, locations);
    }

private:
    FunctionsLocations* locations;

};

} // namespace borealis

#endif /* LOCATEFUNCTIONSACTION_H_ */
