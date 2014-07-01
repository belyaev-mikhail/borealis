/* 
 * File:   SplitStructVisitor.h
 * Author: maxim
 *
 * Created on 19 Июнь 2014 г., 20:50
 */

#include <clang/AST/ASTContext.h>
#include <clang/AST/ParentMap.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Rewrite/Rewriter.h>

#ifndef SPLITSTRUCTVISITOR_H
#define	SPLITSTRUCTVISITOR_H

namespace borealis {

class SplitStructVisitor : public clang::RecursiveASTVisitor<SplitStructVisitor> {
public:

    SplitStructVisitor(clang::ASTContext* context, clang::SourceManager* sm,
            clang::Rewriter* rewriter) : context(context), sm(sm), rewriter(rewriter) {}
    
    bool VisitFunctionDecl(clang::FunctionDecl* s);

private:
    clang::ASTContext* context;
    clang::SourceManager* sm;
    clang::Rewriter* rewriter;
};    

} // namespace borealis

#endif	/* SPLITSTRUCTVISITOR_H */

