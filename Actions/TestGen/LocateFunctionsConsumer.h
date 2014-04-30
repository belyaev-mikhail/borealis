/*
 * LocateFunctionsConsumer.h
 *
 *  Created on: Apr 18, 2014
 *      Author: gagarski
 */

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/FileManager.h>

#include "Actions/TestGen/LocateFunctionsVisitor.h"
#include "TestGen/SourceLocations.h"

#ifndef LOCATEFUNCTIONSCONSUMER_H_
#define LOCATEFUNCTIONSCONSUMER_H_


namespace borealis {

class LocateFunctionsConsumer : public clang::ASTConsumer {
public:

    LocateFunctionsConsumer(clang::ASTContext* context, clang::SourceManager* sm,
            llvm::StringRef inFile, FunctionsLocations* locations) :
                visitor(context, sm, inFile, locations) {}

    virtual void HandleTranslationUnit(clang::ASTContext& context) {
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }

private:
    LocateFunctionsVisitor visitor;
};

} // namespace borealis


#endif /* LOCATEFUNCTIONSCONSUMER_H_ */
