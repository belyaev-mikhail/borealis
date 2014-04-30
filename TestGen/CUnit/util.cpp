/*
 * util.cpp
 *
 *  Created on: Apr 29, 2014
 *      Author: gagarski
 */
#include <cctype>
#include <iostream>
#include <memory>

#include "Util/filename_utils.h"

namespace borealis {
namespace util {

bool copyPartOfFile(std::istream& from, std::ostream& to, unsigned int n, unsigned int& rdBytes) {
    const int bufSize = 1024;
    std::unique_ptr<char> fileBuf(new char[bufSize + 1]);
    int toRead = n;
    while (toRead > 0) {
        int sz = std::min(bufSize, toRead);
        from.read(fileBuf.get(), sz);
        if (!from.good()) {
            return false;
        }
        fileBuf.get()[sz] = '\0';
        toRead -= sz;
        rdBytes+= sz;
        to << fileBuf.get();
    }
    return true;
}

bool copyUntilChar(std::istream& from, std::ostream& to, char stop, unsigned int& rdBytes) {
    char last;
    do {
        last = from.get();
        if (!from.good()) {
            return false;
        }
        rdBytes++;
        to << last;
    } while (last != stop);
    return true;
}

bool copyWhitespaces(std::istream& from, std::ostream& to, unsigned int& rdBytes, int& last) {
    do {
        last = from.get();
        if (from.eof())
            break;
        rdBytes++;
        if (!std::isspace(last))
            break;
        to << static_cast<char>(last);
    } while (true);
    return true;
}
} // namespace util
} // namespace borealis
