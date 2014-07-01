/* 
 * File:   filename_utils.cpp
 * Author: maxim
 *
 * Created on 28 Март 2014 г., 11:07
 */

#include <llvm/ADT/SmallString.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/PathV2.h>

#include "Util/filename_utils.h"
#include "util.hpp"

namespace borealis {
namespace util {

std::string getRelativePath(llvm::StringRef base, llvm::StringRef source, llvm::StringRef target) {
    llvm::SmallString<256> s = source;
    llvm::SmallString<256> t = target;
    std::string result;
    
    if (!llvm::sys::path::is_absolute(s.str())) {
        s = base;
        llvm::sys::path::append(s, source);
    }
    if (!llvm::sys::path::is_absolute(t.str())) {
        t = base;
        llvm::sys::path::append(t, target);
    }
    
    s = normalizePath(s);
    t = normalizePath(t);
        
    auto sIt = llvm::sys::path::begin(s);
    auto tIt = llvm::sys::path::begin(t);
    
    // skip common part of paths
    while (*sIt == *tIt && sIt != llvm::sys::path::end(s) && tIt != llvm::sys::path::end(t)) {
        ++sIt;
        ++tIt;
    }

    // ignore last element of paths (filename)
    auto sEnd = --llvm::sys::path::end(s);
    auto tEnd = --llvm::sys::path::end(t);
    
    if (tIt != llvm::sys::path::end(t)) {
        while (tIt != tEnd) {
            result += "../";
            ++tIt;
        }
    }
    
    if (sIt != llvm::sys::path::end(s)) {
        while (sIt != sEnd) {
            result += *sIt;
            result += "/";
            ++sIt;
        }

        // append filename
        result += *sEnd;
    }
    
    return result;
}

std::string getAbsolutePath(llvm::StringRef base, llvm::StringRef source) {
    if (llvm::sys::path::is_absolute(source)) {
        return normalizePath(source);
    }
    
    llvm::SmallString<256> b = base;
    
    llvm::sys::path::append(b, source);
    
    return normalizePath(b.str());
}

std::string normalizePath(llvm::StringRef path) { 
    
    std::vector<std::string> result;
    auto isAbs = llvm::sys::path::is_absolute(path);
    int preUp = 0;

    for (const auto& s: util::view(llvm::sys::path::begin(path), llvm::sys::path::end(path))) {
        if (s == "..") {
            if (result.empty()) {
                preUp++;
            } else {
                result.pop_back();
            }
        } else if (s == "/" || s == "" || s == ".") {
        } else {
            result.push_back(s);
        }
    }
    
    if (isAbs) {
        result.insert(result.begin(), "");
    } else if (preUp > 0){
        result.insert(result.begin(), preUp, "..");
    }

    auto resultStr = result[0];
    
    for (size_t i = 1; i < result.size(); i++) {
        resultStr += "/" + result[i];
    }
    
    return resultStr;
}

} // namespace util
} // namespace borealis
