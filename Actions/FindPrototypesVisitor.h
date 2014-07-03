/* 
 * File:   FindPrototypesVisitor.h
 * Author: maxim
 *
 * Created on 25 Март 2014 г., 12:18
 */

#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceManager.h>

#include "TestGen/FunctionsInfoData.h"

#ifndef FINDPROTOTYPESVISITOR_H
#define	FINDPROTOTYPESVISITOR_H

namespace borealis {

class FindPrototypesVisitor : public clang::RecursiveASTVisitor<FindPrototypesVisitor> {
public:

    FindPrototypesVisitor(clang::ASTContext* context, clang::SourceManager* sm,
            FunctionsInfoData* fInfoData) : context(context), sm(sm), fInfoData(fInfoData) {}
    
    bool VisitFunctionDecl(clang::FunctionDecl* s);

private:
    clang::ASTContext* context;
    clang::SourceManager* sm;
    FunctionsInfoData* fInfoData;

};    

} // namespace borealis

#endif	/* FINDPROTOTYPESVISITOR_H */

