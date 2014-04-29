/*
 * util.h
 *
 *  Created on: Feb 27, 2014
 *      Author: sam
 */

#ifndef TESTGENUTIL_H_
#define TESTGENUTIL_H_

#include <llvm/Function.h>

namespace borealis {
namespace util {

bool shouldSkipTest(llvm::Function* F);

} // namespace borealis
} // namespace util

#endif /* TESTGENUTIL_H_ */
