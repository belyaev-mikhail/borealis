/*
 * LocateFunctionsVisitor.h
 *
 *  Created on: Apr 18, 2014
 *      Author: gagarski
 */

#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/FileManager.h>

#include "TestGen/SourceLocations.h"

#ifndef LOCATEFUNCTIONSVISITOR_H_
#define LOCATEFUNCTIONSVISITOR_H_

namespace borealis {

class LocateFunctionsVisitor : public clang::RecursiveASTVisitor<LocateFunctionsVisitor> {
public:

    LocateFunctionsVisitor(clang::ASTContext* context, clang::SourceManager* sm,
            llvm::StringRef inFile, FunctionsLocations* locations) :
                context(context), sm(sm), inFile(inFile), locations(locations) {}

    bool VisitFunctionDecl(clang::FunctionDecl* s);

private:
    clang::ASTContext* context;
    clang::SourceManager* sm;
    llvm::StringRef inFile;
    FunctionsLocations* locations;

};

} // namespace borealis



#endif /* LOCATEFUNCTIONSVISITOR_H_ */
