#include <llvm/Analysis/DebugInfo.h>

#include "TestGen/Util/c_types.h"

namespace borealis {
namespace util {

std::string getCType(llvm::DIType type, CTypeModifiersPolicy fullModifiers) {
    std::string postfix;
    std::string prefix;
    bool breakFlag = false;
    while (!breakFlag && !type.isBasicType()) {
        switch (type.getTag()) {
            case llvm::dwarf::DW_TAG_const_type:
                if (fullModifiers == CTypeModifiersPolicy::KEEP) {
                    postfix = " const" + postfix;
                }
                break;
            case llvm::dwarf::DW_TAG_pointer_type:
                postfix = " *" + postfix;
                break;
            case llvm::dwarf::DW_TAG_structure_type:
                prefix += "struct ";
                breakFlag = true;
                break;
            case llvm::dwarf::DW_TAG_typedef:
                breakFlag = true;
                break;
        }
        if (!breakFlag) {
            type = llvm::DIDerivedType(type).getTypeDerivedFrom();
        }
    }
    return prefix + type.getName().str() + postfix;
}

} /* namespace util */
} /* namespace borealis */
