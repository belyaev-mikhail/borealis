#include <llvm/Analysis/DebugInfo.h>

#include "TestGen/Util/c_types.h"

namespace borealis {
namespace util {

std::string getCType(const llvm::DIType * type, bool fullModifiers) {
    std::string result;
    while (type->isDerivedType()) {
        switch (type->getTag()) {
            case llvm::dwarf::DW_TAG_const_type:
                if (fullModifiers) {
                        result = " const" + result;
                }
                break;
            case llvm::dwarf::DW_TAG_pointer_type:
                result = " *" + result;
                break;
        }
        auto dt = llvm::DIDerivedType(*type).getTypeDerivedFrom();
        type = &dt;
    }
    return type->getName().str() + result;
}

} /* namespace util */
} /* namespace borealis */
