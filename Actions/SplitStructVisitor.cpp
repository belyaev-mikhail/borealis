/* 
 * File:   SplitStructVisitor.cpp
 * Author: maxim
 * 
 * Created on 19 Июнь 2014 г., 20:50
 */

#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/Lex/Lexer.h>
#include <llvm/Support/Debug.h>

#include "Actions/SplitStructVisitor.h"
#include "TestGen/Util/util.h"
#include "Util/util.h"

namespace borealis {

std::string getStructureParamReplace(clang::RecordDecl* sd, std::string& qualsStr, std::string prefix) {
    
    std::string result;
    for (auto f: util::view(sd->field_begin(), sd->field_end())) {
        if (f->getType()->isStructureType()) {
            result += getStructureParamReplace(f->getType()->getAsStructureType()->getDecl(),
                    qualsStr, prefix + f->getNameAsString() + "_") + ", ";
        } else {
            result += qualsStr + f->getType().getAsString() + " " +
                    prefix + f->getNameAsString() + ", ";
        }
    }
    result.erase(result.end() - 2, result.end());
    
    return result;
}

std::string getStructureInit(clang::RecordDecl* sd, std::string prefix) {
    
    std::string result;
    for (auto f: util::view(sd->field_begin(), sd->field_end())) {
        if (f->getType()->isStructureType()) {
            result += getStructureInit(f->getType()->getAsStructureType()->getDecl(),
                    prefix + f->getNameAsString() + "_") + ", ";
        } else {
            result += prefix + f->getNameAsString() + ", ";
        }
    }
    result.erase(result.end() - 2, result.end());
    
    return "{" + result + "}";
}

std::string getStructureArgReplace(clang::RecordDecl* sd, std::string prefix) {
    
    std::string result;
    for (auto f: util::view(sd->field_begin(), sd->field_end())) {
        if (f->getType()->isStructureType()) {
            result += getStructureArgReplace(f->getType()->getAsStructureType()->getDecl(),
                    prefix + f->getNameAsString() + ".") + ", ";
        } else {
            result += prefix + f->getNameAsString() + ", ";
        }
    }
    result.erase(result.end() - 2, result.end());
    
    return result;
}

bool SplitStructVisitor::VisitFunctionDecl(clang::FunctionDecl* s) {
    if (util::isSystem(s->getLocation(), *sm)) {
        return true;
    }
    
    if (!s->doesThisDeclarationHaveABody()) {
        return true;
    }
    
    auto hasStructs = false;
    auto stubFuction = rewriter->getRewrittenText(clang::SourceRange(s->getLocStart(), s->getBody()->getLocStart()));
    
    for (auto p : util::view(s->param_begin(), s->param_end())) {
        if (p->getType()->isStructureType()) {
            auto sd = p->getType()->getAsStructureType()->getDecl();
            auto quals = p->getType().getQualifiers();
            auto qualsStr = (quals.empty() ? "" : quals.getAsString() + " ");
            
            auto tempArgs = getStructureParamReplace(sd, qualsStr, "__" + p->getNameAsString() + "_");
            auto tempInit = getStructureInit(sd, "__" + p->getNameAsString() + "_");
            
            tempInit = p->getType().getAsString() + " " + 
                    p->getNameAsString() + " = " + tempInit + ";\n";
            
            rewriter->ReplaceText(p->getSourceRange(), tempArgs);
            if (!s->getBody()->children().empty()) {
                rewriter->InsertText(s->getBody()->child_begin()->getLocStart(), tempInit, false, true);
            }
            
            hasStructs = true;
        }
    }
    
    if (hasStructs) {
        rewriter->InsertText(s->getNameInfo().getBeginLoc(), "__", false, false);
        
        stubFuction = "\n\n" + stubFuction + "\n";
        
        std::string origFunctionArgs;
        
        if (s->getNumParams() > 0) {
            for (auto p : util::view(s->param_begin(), s->param_end())) {
                if (p->getType()->isStructureType()) {
                    auto sd = p->getType()->getAsStructureType()->getDecl();
                    origFunctionArgs += getStructureArgReplace(sd, p->getNameAsString() + ".");
                } else {
                    origFunctionArgs += p->getNameAsString();
                }
                origFunctionArgs += ", ";
            }
        
            origFunctionArgs.erase(origFunctionArgs.end() - 2, origFunctionArgs.end());
        }
        
        stubFuction += "    ";
        if (!s->getResultType()->isVoidType()) {
            stubFuction += "return ";
        }
        stubFuction += "__" + s->getNameAsString() + "(" + origFunctionArgs + ");\n";
        
        stubFuction += "}\n";
        
        rewriter->InsertText(s->getLocEnd().getLocWithOffset(1), stubFuction, true, true);
    }
    
    return true;
}

} // namespace borealis

