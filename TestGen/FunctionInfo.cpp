/* 
 * File:   FunctionInfo.cpp
 * Author: maxim
 * 
 * Created on 17 Май 2014 г., 11:38
 */

#include "Term/Term.h"
#include "TestGen/FunctionInfo.h"
#include "Util/collections.hpp"

namespace borealis {

FunctionInfo::FunctionInfo(const llvm::Function* f, SlotTracker* st) : f(f) {
    auto fn = FactoryNest(st);
    initialize(st, &fn);
}

FunctionInfo::FunctionInfo(const llvm::Function* f, SlotTracker* st, FactoryNest* fn) : f(f) {
    initialize(st, fn);
}

void FunctionInfo::initialize(SlotTracker* st, FactoryNest* fn) {
    st->initialize();
    
    std::unordered_map<unsigned, std::pair<std::string, llvm::DIType>> args;
    
    for (const auto& mdn: util::view(st->mdn_begin(), st->mdn_end())) {
        auto d = llvm::DIDescriptor(mdn.first);
        if (d.getTag() == llvm::dwarf::DW_TAG_arg_variable) {
            auto v = llvm::DIVariable(mdn.first);
            args.emplace(v.getArgNumber() - 1, std::make_pair(v.getName().str(), v.getType()));
        } else if (d.getTag() == llvm::dwarf::DW_TAG_subprogram) {
            auto sp = llvm::DISubprogram(mdn.first);
            if (sp.getFunction() == f) {
                auto rt = sp.getType().getTypeArray().getElement(0);
                returnType = llvm::DIType(rt);
            }
        }
    }
    
    this->args.resize(args.size());
    
    auto arg = f->arg_begin();
    for (unsigned i = 0; i < args.size(); i++, arg++) {
        auto arg_ = const_cast<llvm::Argument *>(&(*arg));
        auto argTerm = fn->Term->getArgumentTerm(arg_);
        
        auto& _args = args[i];
        
        this->args[i] = {_args.first, _args.second, argTerm};
    }
}

} // namespace borealis

