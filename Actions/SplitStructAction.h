/* 
 * File:   SplitStructAction.h
 * Author: maxim
 *
 * Created on 19 Июнь 2014 г., 20:09
 */

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Rewrite/Rewriter.h>

#include "Actions/SplitStructConsumer.h"
#include "Util/util.hpp"

#ifndef SPLITSTRUCTACTION_H
#define	SPLITSTRUCTACTION_H

namespace borealis {

class SplitStructAction : public clang::ASTFrontendAction {
public:
    
    explicit SplitStructAction(clang::Rewriter* rewriter) : rewriter(rewriter) {}
    
    virtual clang::ASTConsumer* CreateASTConsumer(clang::CompilerInstance& compiler,
                                                  llvm::StringRef inFile) {
        util::use(inFile);
        return new SplitStructConsumer(&compiler.getASTContext(),
                &compiler.getSourceManager(), rewriter);
    }
    
private:
    clang::Rewriter* rewriter;
};

} // namespace borealis

#endif	/* SPLITSTRUCTACTION_H */

