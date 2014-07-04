/* 
 * File:   FunctionInfo.cpp
 * Author: maxim
 * 
 * Created on 17 Май 2014 г., 11:38
 */

#include "Term/Term.h"
#include "TestGen/FunctionInfo.h"
#include "TestGen/Util/c_types.h"
#include "Util/collections.hpp"

namespace borealis {

static std::string formatToType(Term::Ptr trm, borealis::DIType type) {
    while(DIAlias alias = type) type = alias.getOriginal();

    if(auto integer = llvm::dyn_cast<OpaqueIntConstantTerm>(trm)) {
        llvm::APInt app{ type.getSizeInBits(), integer->getValue(), !type.isUnsignedDIType() };
        return app.toString(10, !type.isUnsignedDIType());
    }
    return trm->getName();
}

FunctionInfo::FunctionInfo(const FunctionInfo& orig) : f(orig.f), stub(orig.stub),
        fake(orig.fake), stubFunc(orig.stubFunc), returnType(orig.returnType),
        args(orig.args), hasPtrs(orig.hasPtrs) {
    for (auto& a: args) {
        a.parent = this;
    }
}

FunctionInfo::FunctionInfo(FunctionInfo&& orig) : f(std::move(orig.f)),
        stub(std::move(orig.stub)), fake(std::move(orig.fake)),
        stubFunc(std::move(orig.stubFunc)), returnType(std::move(orig.returnType)),
        args(std::move(orig.args)), hasPtrs(std::move(orig.hasPtrs)) {
    for (auto& a: args) {
        a.parent = this;
    }
}

FunctionInfo::FunctionInfo(const llvm::Function* f, SlotTracker* st) : f(f) {
    auto fn = FactoryNest(st);
    initialize(st, &fn);
}

FunctionInfo::FunctionInfo(const llvm::Function* f, SlotTracker* st, FactoryNest* fn) : f(f) {
    initialize(st, fn);
}

bool checkStructForPtrs(const DIStructType& s) {
    auto mems = s.getMembers();
    for (unsigned i = 0; i < mems.getNumElements(); i++) {
        auto memType = mems.getElement(i).getType();
        switch (memType.getTag()) {
            case llvm::dwarf::DW_TAG_structure_type:
                if (checkStructForPtrs(DIStructType(memType))) {
                    return true;
                }
                break;
            case llvm::dwarf::DW_TAG_pointer_type:
                return true;
                break;
        }
    }
    return false;
}

void FunctionInfo::initialize(SlotTracker* st, FactoryNest* fn) {
    st->initialize();
    
    std::unordered_map<unsigned, std::pair<std::string, llvm::DIType>> args;
    
    stub = false;
    
    fake = f->getName().startswith("__");
    
    hasPtrs = false;
    
    for (const auto& mdn: util::view(st->mdn_begin(), st->mdn_end())) {
        auto d = llvm::DIDescriptor(mdn.first);
        if (d.getTag() == llvm::dwarf::DW_TAG_arg_variable) {
            auto v = llvm::DIVariable(mdn.first);
            auto sp = llvm::DISubprogram(v.getContext());
            if (sp.getFunction() == f) {
                args.emplace(v.getArgNumber() - 1, std::make_pair(v.getName().str(), v.getType()));
                switch (v.getType().getTag()) {
                    case llvm::dwarf::DW_TAG_structure_type:
                        stub = true;
                        if (checkStructForPtrs(DIStructType(v.getType()))) {
                            hasPtrs = true;
                        }
                        break;
                    case llvm::dwarf::DW_TAG_pointer_type:
                        hasPtrs = true;
                        break;
                }
            }
        } else if (d.getTag() == llvm::dwarf::DW_TAG_subprogram) {
            auto sp = llvm::DISubprogram(mdn.first);
            if (sp.getFunction() == f) {
                auto rt = sp.getType().getTypeArray().getElement(0);
                returnType = llvm::DIType(rt);
            }
        }
    }
    
    this->args.resize(args.size());
    
    for (unsigned i = 0; i < args.size(); i++) {
        auto& _args = args[i];
        
        this->args[i].name = _args.first;
        this->args[i].type = _args.second;
        this->args[i].parent = this;
        this->args[i].idx = i;
    }
    
    if (stub) {
        return;
    }
    
    auto arg = f->arg_begin();
    for (unsigned i = 0; i < args.size(); i++) {
        auto arg_ = const_cast<llvm::Argument *>(&(*arg));
        auto argTerm = fn->Term->getArgumentTerm(arg_);

        this->args[i].term = argTerm;
        arg++;
    }
}

llvm::StringRef FunctionInfo::getName() const {
    if (fake) {
        return stubFunc->f->getName();
    } else {
        return f->getName();
    }
}

llvm::DIType FunctionInfo::getReturnType() const {
    if (fake) {
        return stubFunc->returnType;
    } else {
        return returnType;
    }
}

size_t FunctionInfo::getArgsCount() const {
    if (fake) {
        return stubFunc->args.size();
    } else {
        return args.size();
    }
}

FunctionInfo::ArgInfo FunctionInfo::getArg(int i) const {
    if (fake) {
        return stubFunc->args[i];
    } else {
        return args[i];
    }
}

std::vector<FunctionInfo::ArgInfo>::const_iterator FunctionInfo::begin() const {
    if (fake) {
        return stubFunc->args.begin();
    } else {
        return args.begin();
    }
}

std::vector<FunctionInfo::ArgInfo>::const_iterator FunctionInfo::end() const {
    if (fake) {
        return stubFunc->args.end();
    } else {
        return args.end();
    }
}

const llvm::Function* FunctionInfo::getFunction() const {
    if (fake) {
        return stubFunc->f;
    } else {
        return f;
    }
}

bool FunctionInfo::hasPtrArgs() const {
    if (fake) {
        return stubFunc->hasPtrs;
    } else {
        return hasPtrs;
    }
};

void FunctionInfo::setStubFunc(FunctionInfo* stub) {
    if (fake && stub->stub) {
        stubFunc = stub;
        stub->realFunc = this;
    }
}

std::string getStructureValue(const DIStructType& s, std::vector<FunctionInfo::ArgInfo>::iterator& argIt, const TestCase& cs) {
    auto mems = s.getMembers();
    std::string result;
    if (mems.getNumElements() > 0) {
        for (unsigned i = 0; i < mems.getNumElements(); i++) {
            if (mems.getElement(i).getType().getTag() == llvm::dwarf::DW_TAG_structure_type) {
                auto s = DIStructType(mems.getElement(i).getType());
                result += getStructureValue(s, argIt, cs) + ", ";
            } else {
                result += formatToType(cs.getValue(argIt->term), argIt->type) + ", ";
                argIt++;
            }
        }
        result.erase(result.end() - 2, result.end());
    }
    return "{" + result + "}";
}

std::string FunctionInfo::ArgInfo::getValue(const TestCase& cs) const {
    if (parent->isStub()) {
        int realIdx = 0;
        
        for (int i = 0; i < idx; i++) {
            auto arg = parent->args[i];
            if (arg.type.getTag() == llvm::dwarf::DW_TAG_structure_type) {
                auto s = DIStructType(arg.type);
                realIdx += s.getMembers().getNumElements();
            } else {
                realIdx++;
            }
        }
        
        if (type.getTag() == llvm::dwarf::DW_TAG_structure_type) {
            auto s = DIStructType(type);
            auto argIt = parent->realFunc->args.begin() + realIdx;
            return getStructureValue(s, argIt, cs);
        } else {
            auto arg = parent->realFunc->args[realIdx];
            return formatToType(cs.getValue(arg.term), arg.type);
        }
    } else {
        return formatToType(cs.getValue(term), type);
    }
}

} // namespace borealis
