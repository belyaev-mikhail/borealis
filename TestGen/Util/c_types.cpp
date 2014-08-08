#include "TestGen/Util/c_types.h"
#include "Codegen/llvm.h"

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
            case llvm::dwarf::DW_TAG_union_type:
                prefix += "union ";
                breakFlag = true;
                break;
            case llvm::dwarf::DW_TAG_enumeration_type:
                prefix += "enum ";
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

bool isTypesEqual(llvm::DIType cType, const llvm::Type* llvmType) {
    bool breakFlag = false;
    while (!breakFlag && !cType.isBasicType()) {
        switch (cType.getTag()) {
            case llvm::dwarf::DW_TAG_pointer_type:
                if (llvmType->isPointerTy()) {
                    llvmType = llvmType->getPointerElementType();
                } else {
                    return false;
                }
                // no break need here
            case llvm::dwarf::DW_TAG_typedef:
            case llvm::dwarf::DW_TAG_const_type:
                cType = llvm::DIDerivedType(cType).getTypeDerivedFrom();
                break;
            default:
                breakFlag = true;
                break;
        }
    }
    
    if (cType.isBasicType()) {
        if (llvmType->isPrimitiveType() || llvmType->isIntegerTy()) {
            return cType.getSizeInBits() == llvmType->getPrimitiveSizeInBits();
        } else {
            return false;
        }
    } else {
        if (cType.getTag() == llvm::dwarf::DW_TAG_structure_type) {
            auto cStructMembers = DIStructType(cType).getMembers();
            if (llvmType->isStructTy() && cStructMembers.getNumElements() == llvmType->getStructNumElements()) {
                for (unsigned i = 0; i < cStructMembers.getNumElements(); i++) {
                    if (!isTypesEqual(cStructMembers.getElement(i).getType(), llvmType->getStructElementType(i))) {
                        return false;
                    }
                }
            } else {
                return false;
            }
        }
    }
    
    return true;
}

} /* namespace util */
} /* namespace borealis */
