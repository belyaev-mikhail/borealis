/* 
 * File:   SplitStructConsumer.h
 * Author: maxim
 *
 * Created on 19 Июнь 2014 г., 20:45
 */

#ifndef SPLITSTRUCTCONSUMER_H
#define	SPLITSTRUCTCONSUMER_H

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Rewrite/Rewriter.h>

#include "Actions/SplitStructVisitor.h"

namespace borealis {

class SplitStructConsumer : public clang::ASTConsumer {
public:

    SplitStructConsumer(clang::ASTContext* context, clang::SourceManager* sm,
            clang::Rewriter* rewriter) : visitor(context, sm, rewriter) {}

    virtual void HandleTranslationUnit(clang::ASTContext& context) {
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
        
private:
    SplitStructVisitor visitor;
};

} // namespace borealis

#endif	/* SPLITSTRUCTCONSUMER_H */

