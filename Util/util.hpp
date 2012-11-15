/*
 * util.hpp
 *
 *  Created on: Aug 22, 2012
 *      Author: belyaev
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <llvm/Support/ManagedStatic.h>

#include <cstddef>

#include "Logging/logger.hpp"
#include "streams.hpp"

////////////////////////////////////////////////////////////////////////////////
//
// borealis::util
//
////////////////////////////////////////////////////////////////////////////////

namespace borealis {
namespace util {

template<typename RetTy = void>
RetTy sayonara(std::string file, int line, std::string where, std::string reason) {
    logging::errs() << file << ":" << toString(line) << " |" << where << "| "
            << reason << logging::endl;

    llvm::llvm_shutdown();
    std::exit(EXIT_FAILURE);
}

} // naemspace util
} // naemspace borealis

#endif /* UTIL_HPP_ */
