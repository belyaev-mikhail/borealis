/* 
 * File:   FindPrototypesAction.h
 * Author: maxim
 *
 * Created on 25 Март 2014 г., 11:33
 */

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>

#include "Actions/FindPrototypesConsumer.h"
#include "TestGen/PrototypesInfo.h"
#include "Util/util.hpp"

#ifndef FINDPROTOTYPESACTION_H
#define	FINDPROTOTYPESACTION_H

namespace borealis {

class FindPrototypesAction : public clang::ASTFrontendAction {
public:
    
    explicit FindPrototypesAction(PrototypesInfo* prototypes) : prototypes(prototypes) {}
    
    virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance& compiler,
                                                  llvm::StringRef inFile) {
        util::use(inFile);
        return new FindPrototypesConsumer(&compiler.getASTContext(),
                &compiler.getSourceManager(), prototypes);
    }
    
private:
    PrototypesInfo* prototypes;
    
};

} // namespace borealis

#endif	/* FINDPROTOTYPESACTION_H */

