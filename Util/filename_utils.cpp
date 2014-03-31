/* 
 * File:   filename_utils.cpp
 * Author: maxim
 *
 * Created on 28 Март 2014 г., 11:07
 */

#include <llvm/ADT/SmallString.h>
#include <llvm/Support/PathV2.h>
#include <llvm/Support/Debug.h>

#include "Util/filename_utils.h"

namespace borealis {
namespace util {

std::string getRelativePath(llvm::StringRef base, llvm::StringRef source, llvm::StringRef target) {
    using namespace llvm::sys::path;
    
    llvm::SmallString<256> s = source;
    llvm::SmallString<256> t = target;
    std::string result;
    
    if (is_absolute(t.str()) && !is_absolute(s.str())) {
        s = base;
        append(s, source);
    } else if (is_absolute(s.str()) && !is_absolute(t.str())) {
        t = base;
        append(t, target);
    }
    
    auto sIt = begin(s);
    auto tIt = begin(t);
    
    // skip common part of paths
    while (*sIt == *tIt) {
        ++sIt;
        ++tIt;
    }

    // ignore last element of paths (filename)
    auto sEnd = --end(s);
    auto tEnd = --end(t);
    
    while (tIt != tEnd) {
        result += "../";
        ++tIt;
    }
    
    while (sIt != sEnd) {
        result += *sIt;
        result += "/";
        ++sIt;
    }
    
    // append filename
    result += *sEnd;
    
    return result;
}

} // namespace util
} // namespace borealis
