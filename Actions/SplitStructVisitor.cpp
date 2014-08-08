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

std::string getParamReplace(clang::QualType t, std::string& qualsStr, std::string name) {
    if (t->isStructureType()) {
        std::string result;
        auto* sd = t->getAsStructureType()->getDecl();
        if (!sd->field_empty()) {
            for (auto f: util::view(sd->field_begin(), sd->field_end())) {
                auto fName = f->getNameAsString();
                util::replaceAll("_", "__", fName);
                result += getParamReplace(f->getType(), qualsStr, name + "_p_" + fName) + ", ";
            }
            result.erase(result.end() - 2, result.end());
        }
        return result;
    } else if (t->isConstantArrayType()) {
        std::string result;
        auto at = llvm::dyn_cast<clang::ConstantArrayType>(t);
        auto eType = at->getElementType();
        int arraySize = *(at->getSize().getRawData());
        if (arraySize > 0) {
            for (int i = 0; i < arraySize; i++) {
                result += getParamReplace(eType, qualsStr, name + "_ib_" + util::toString(i) + "_ie_") + ", ";
            }
            result.erase(result.end() - 2, result.end());
        }
        return result;
    } else {
        return qualsStr + t.getAsString() + " " + name;
    }
}

std::string getInit(clang::QualType t, std::string name) {
    if (t->isStructureType()) {
        std::string result;
        auto* sd = t->getAsStructureType()->getDecl();
        if (!sd->field_empty()) {
            for (auto f: util::view(sd->field_begin(), sd->field_end())) {
                auto fName = f->getNameAsString();
                util::replaceAll("_", "__", fName);
                result += getInit(f->getType(), name + "_p_" + fName) + ", ";
            }
            result.erase(result.end() - 2, result.end());
        }
        return "{" + result + "}";
    } else if (t->isConstantArrayType()) {
        std::string result;
        auto at = llvm::dyn_cast<clang::ConstantArrayType>(t);
        auto eType = at->getElementType();
        int arraySize = *(at->getSize().getRawData());
        if (arraySize > 0) {
            for (int i = 0; i < arraySize; i++) {
                result += getInit(eType, name + "_ib_" + util::toString(i) + "_ie_") + ", ";
            }
            result.erase(result.end() - 2, result.end());
        }
        return "{" + result + "}";
    } else {
        return name;
    }
}

std::string getArgReplace(clang::QualType t, std::string name) {
    if (t->isStructureType()) {
        std::string result;
        auto* sd = t->getAsStructureType()->getDecl();
        if (!sd->field_empty()) {
            for (auto f: util::view(sd->field_begin(), sd->field_end())) {
                result += getArgReplace(f->getType(), name + "." + f->getNameAsString()) + ", ";
            }
            result.erase(result.end() - 2, result.end());
        }
        return result;
    } else if (t->isConstantArrayType()) {
        std::string result;
        auto at = llvm::dyn_cast<clang::ConstantArrayType>(t);
        auto eType = at->getElementType();
        int arraySize = *(at->getSize().getRawData());
        if (arraySize > 0) {
            for (int i = 0; i < arraySize; i++) {
                result += getArgReplace(eType, name + "[" + util::toString(i) + "]") + ", ";
            }
            result.erase(result.end() - 2, result.end());
        }
        return result;
    } else {
        return name;
    }
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
        auto t = p->getType();
        auto name = p->getNameAsString();
        util::replaceAll("_", "__", name);
        if (t->isStructureType()) {
            auto quals = t.getQualifiers();
            auto qualsStr = (quals.empty() ? "" : quals.getAsString() + " ");
            
            auto tempArgs = getParamReplace(t, qualsStr, "__" + name);
            auto tempInit = getInit(t, "__" + name);
            
            tempInit = t.getAsString() + " " + 
                    name + " = " + tempInit + ";\n";
            
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
                origFunctionArgs += getArgReplace(p->getType(), p->getNameAsString()) + ", ";
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

