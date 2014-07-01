/* 
 * File:   FunctionInfo.h
 * Author: maxim
 *
 * Created on 17 Май 2014 г., 11:38
 */

#ifndef FUNCTIONINFO_H
#define	FUNCTIONINFO_H

#include <unordered_map>
#include <vector>

#include <llvm/Analysis/DebugInfo.h>
#include <llvm/Function.h>

#include "Factory/Nest.h"
#include "TestGen/TestCase.h"
#include "Util/slottracker.h"

namespace borealis {

class FunctionInfo {
public:
    FunctionInfo() = delete;
    FunctionInfo(const FunctionInfo& orig);
    FunctionInfo(FunctionInfo&& orig);
    
    FunctionInfo(const llvm::Function* f, SlotTracker* st);
    FunctionInfo(const llvm::Function* f, SlotTracker* st, FactoryNest* fn);
    
    struct ArgInfo {
        std::string name;
        llvm::DIType type;
        Term::Ptr term;
        
        FunctionInfo* parent;
        int idx;
        
        std::string getValue(const TestCase& cs) const;
    };
    
    llvm::StringRef getName() const;
    llvm::DIType getReturnType() const;
    size_t getArgsCount() const;
    ArgInfo getArg(int i) const;
    std::vector<ArgInfo>::const_iterator begin() const;
    std::vector<ArgInfo>::const_iterator end() const;
    const llvm::Function* getFunction() const;
    
    llvm::StringRef getRealName() const {return f->getName();}
    size_t getRealArgsCount() const {return args.size();}
    ArgInfo getRealArg(int i) const {return args[i];}
    std::vector<ArgInfo>::const_iterator begin_real_args() const {return args.begin();}
    std::vector<ArgInfo>::const_iterator end_real_args() const {return args.end();}
    const llvm::Function* getRealFunction() const {return f;}
    
    bool isStub() const {return stub;}
    
    bool isFake() const {return fake;}
    void setStubFunc(FunctionInfo* stub);
    
private:
    const llvm::Function* f;
    
    bool stub;
    FunctionInfo* realFunc;
    
    bool fake;
    FunctionInfo* stubFunc;
    
    llvm::DIType returnType;
    std::vector<ArgInfo> args;
    
    void initialize(SlotTracker* st, FactoryNest* fn);

};

} // namespace borealis

#endif	/* FUNCTIONINFO_H */

