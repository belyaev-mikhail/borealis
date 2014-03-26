/* 
 * File:   FindPrototypesConsumer.h
 * Author: maxim
 *
 * Created on 25 Март 2014 г., 12:00
 */

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/FileManager.h>

#include "Actions/FindPrototypesVisitor.h"
#include "TestGen/PrototypesInfo.h"

#ifndef FINDPROTOTYPESCONSUMER_H
#define	FINDPROTOTYPESCONSUMER_H

namespace borealis {

class FindPrototypesConsumer : public clang::ASTConsumer {
public:

    FindPrototypesConsumer(clang::ASTContext* context, clang::SourceManager* sm,
            PrototypesInfo* prototypes) : visitor(context, sm, prototypes) {}

    virtual void HandleTranslationUnit(clang::ASTContext& context) {
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
        
private:
    FindPrototypesVisitor visitor;
};

} // namespace borealis

#endif	/* FINDPROTOTYPESCONSUMER_H */

