/* 
 * File:   c_types.h
 * Author: maxim
 *
 * Created on 4 Март 2014 г., 13:41
 */

#ifndef C_TYPES_H
#define	C_TYPES_H

#include <llvm/Analysis/DebugInfo.h>
#include <llvm/Type.h>

namespace borealis {
namespace util {

enum class CTypeModifiersPolicy { KEEP, DISCARD };
std::string getCType(llvm::DIType type, CTypeModifiersPolicy fullModifiers);

bool isTypesEqual(llvm::DIType cType, const llvm::Type* llvmType);

} /* namespace util */
} /* namespace borealis */

#endif	/* C_TYPES_HPP */

