#include <llvm/Analysis/DebugInfo.h>

#include "TestGen/Util/c_types.h"

namespace borealis {
namespace util {

std::string getCType(llvm::DIType type, CTypeModifiersPolicy fullModifiers) {
    std::string result;
    while (type.isDerivedType()) {
        switch (type.getTag()) {
            case llvm::dwarf::DW_TAG_const_type:
                if (fullModifiers == CTypeModifiersPolicy::KEEP) {
                    result = " const" + result;
                }
                break;
            case llvm::dwarf::DW_TAG_pointer_type:
                result = " *" + result;
                break;
        }
        type = llvm::DIDerivedType(type).getTypeDerivedFrom();
    }
    return type.getName().str() + result;
}

} /* namespace util */
} /* namespace borealis */
