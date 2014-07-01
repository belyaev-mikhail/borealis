/*
 * util.cpp
 *
 *  Created on: Feb 27, 2014
 *      Author: sam
 */

#include <clang/Basic/SourceManager.h>

#include "Codegen/intrinsics_manager.h"
#include "TestGen/Util/util.h"

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

bool isSystem(const clang::SourceLocation& loc, clang::SourceManager& sm) {
    return sm.isInSystemHeader(loc) || sm.isInSystemMacro(loc) || sm.isInExternCSystemHeader(loc);
}

} // namespace borealis
} // namespace util




