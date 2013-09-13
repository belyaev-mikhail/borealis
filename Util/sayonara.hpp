/*
 * sayonara.hpp
 *
 *  Created on: Aug 30, 2013
 *      Author: ice-phoenix
 */

#ifndef SAYONARA_HPP_
#define SAYONARA_HPP_

#include "Util/streams.hpp"

namespace borealis {
namespace util {

[[noreturn]] void diediedie(const char*);

template<typename RetTy = void>
RetTy sayonara(const std::string& file, int line, const std::string& where, const std::string& reason) {
    using namespace borealis::logging;

    std::ostringstream oss;
    oss << file << ":" << std::to_string(line) << "\n"
        << "\t" << where << "\n"
        << "\t" << reason << "\n";
    diediedie(oss.str().c_str());
}

} // namespace util
} // namespace borealis

#endif /* SAYONARA_HPP_ */
