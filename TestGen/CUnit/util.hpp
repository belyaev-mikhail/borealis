/*
 * util.hpp
 *
 *  Created on: Apr 25, 2014
 *      Author: gagarski
 */

#ifndef CUNIT_UTIL_HPP_
#define CUNIT_UTIL_HPP_

#include <unistd.h>

#include "Util/filename_utils.h"

namespace borealis {
namespace util {

bool copyPartOfFile(std::istream& from, std::ostream& to, unsigned int n, unsigned int& rdBytes);
bool copyUntilChar(std::istream& from, std::ostream& to, char stop, unsigned int& rdBytes);
bool copyUntilUnescapedEOL(std::istream& from, std::ostream& to, unsigned int& rdBytes);
bool copyWhitespaces(std::istream& from, std::ostream& to, unsigned int& rdBytes, int& last);

template<class Iter>
std::vector<std::string> getIncludesForFunctions(
        Iter begin, Iter end, PrototypesInfo prototypes) {
    std::unordered_set<std::string> userIncludes;
    for (const auto& f: util::view(begin, end)) {
        auto loc = prototypes.locations.find(f->getName());
        if (loc != prototypes.locations.end()) {
            userIncludes.insert(loc->second);
        }
    }
    std::vector<std::string> includes(userIncludes.begin(), userIncludes.end());
    sort(includes.begin(), includes.end());
    return std::move(includes);
}

template<class Iter>
void writeIncludes(Iter begin, Iter end, std::ostream& os,
        const std::string& baseDirectory, const std::string& moduleName) {
    for (const auto& i: util::view(begin, end)) {
        if (TestDumpPass::includeInMakefile()) {
            os << "#include \"" << i << "\"\n";
        } else if (TestDumpPass::absoluteInclude()) {
            os << "#include \""
               << util::getAbsolutePath(baseDirectory, llvm::StringRef(i))
               << "\"\n";
        } else {
            os << "#include \""
               << util::getRelativePath(baseDirectory, llvm::StringRef(i),
                       llvm::StringRef(TestDumpPass::filePathForModule(moduleName)))
               << "\"\n";
        }
    }
}
} // namespace util
} // namespace borealis

#endif /* CUNIT_UTIL_HPP_ */
