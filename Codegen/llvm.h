/*
 * llvm.h
 *
 *  Created on: Feb 25, 2013
 *      Author: ice-phoenix
 */

#ifndef CODEGEN_LLVM_H_
#define CODEGEN_LLVM_H_

#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Metadata.h>

#include "Util/util.h"

namespace borealis {

void insertBeforeWithLocus(
        llvm::Instruction* what,
        llvm::Instruction* before,
        const Locus& loc);
void setDebugLocusWithCopiedScope(
        llvm::Instruction* to,
        llvm::Instruction* from,
        const Locus& loc);

llvm::MDNode* ptr2MDNode(llvm::LLVMContext& ctx, void* ptr);
void* MDNode2Ptr(llvm::MDNode* ptr);

llvm::StringRef getRawSource(const clang::FileManager& sm, const LocusRange& range);

util::option<std::string> getAsCompileTimeString(llvm::Value* value);

std::list<llvm::Constant*> getAsSeqData(llvm::Constant* value);

} // namespace borealis

#endif /* CODEGEN_LLVM_H_ */
