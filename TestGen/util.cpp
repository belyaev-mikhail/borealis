/*
 * util.cpp
 *
 *  Created on: Feb 27, 2014
 *      Author: sam
 */

#include "Codegen/intrinsics_manager.h"
#include "TestGen/util.h"

namespace borealis {
namespace util {

bool shouldSkipTest(llvm::Function* F) {
    // skip intrinsic functions
    auto& im = IntrinsicsManager::getInstance();
    auto ftype = im.getIntrinsicType(F);
    if (isIntrinsic(ftype) || isBuiltin(ftype) || isAction(ftype))
        return true;

    // XXX sam How to determine main function?
    if (F->getName() == "__main" || F->getName() == "main") // skip main function
        return true;

    return false;
}

} // namespace borealis
} // namespace util




