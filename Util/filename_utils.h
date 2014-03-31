/* 
 * File:   filename_utils.h
 * Author: maxim
 *
 * Created on 28 Март 2014 г., 10:54
 */

#include <llvm/ADT/StringRef.h>

#ifndef FILENAME_UTILS_H
#define	FILENAME_UTILS_H

namespace borealis {
namespace util {

// Return relative path for source relate to target path.
// If source or target are relative they relate to base path.
std::string getRelativePath(llvm::StringRef base, llvm::StringRef source, llvm::StringRef target);

} // namespace util
} // namespace borealis

#endif	/* FILENAME_UTILS_H */

