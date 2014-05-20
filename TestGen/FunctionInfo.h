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
#include "Util/slottracker.h"

namespace borealis {

class FunctionInfo {
public:
    FunctionInfo() = delete;
    FunctionInfo(const FunctionInfo& orig) = default;
    FunctionInfo(FunctionInfo&& orig) = default;
    
    FunctionInfo(const llvm::Function* f, SlotTracker* st);
    FunctionInfo(const llvm::Function* f, SlotTracker* st, FactoryNest* fn);
    
    struct ArgInfo {
        std::string name;
        llvm::DIType type;
        Term::Ptr term;
    };
    
    llvm::StringRef getName() const {return f->getName();}
    llvm::DIType getReturnType() const {return returnType;}
    size_t getArgsCount() const {return args.size();}
    ArgInfo getArg(int i) const {return args[i];}
    std::vector<ArgInfo>::const_iterator begin() const {return args.begin();}
    std::vector<ArgInfo>::const_iterator end() const {return args.end();}
    
private:
    const llvm::Function* f;
    
    llvm::DIType returnType;
    std::vector<ArgInfo> args;
    
    void initialize(SlotTracker* st, FactoryNest* fn);

};

} // namespace borealis

#endif	/* FUNCTIONINFO_H */

