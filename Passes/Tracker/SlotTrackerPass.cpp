/*
 * SlotTrackerPass.cpp
 *
 *  Created on: Oct 4, 2012
 *      Author: belyaev
 */

#include "Passes/Tracker/SlotTrackerPass.h"
#include "Util/passes.hpp"

namespace borealis {

bool SlotTrackerPass::doInitialization(llvm::Module& M) {
    if (!globals) globals.reset(new SlotTracker(&M));
	return false;
}

bool SlotTrackerPass::runOnModule(llvm::Module& M) {
    doInitialization(M);

	for (auto& F : M)
	    funcs[&F] = ptr_t{ new SlotTracker(&F) };

	return false;
}

void SlotTrackerPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
	AU.setPreservesAll();
}

SlotTracker* SlotTrackerPass::getSlotTracker(const llvm::Function* func) const {
	if (func && borealis::util::containsKey(funcs, func)) {
		return funcs.at(func).get();
	} else {
		return nullptr;
	}
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::Module*) const{
	return globals.get();
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::BasicBlock* bb) const{
	return getSlotTracker(bb->getParent());
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::Instruction* inst) const{
	return getSlotTracker(inst->getParent());
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::Argument* arg) const{
	return getSlotTracker(arg->getParent());
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::Function& func) const{
	return getSlotTracker(&func);
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::Module&) const{
	return globals.get();
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::BasicBlock& bb) const{
	return getSlotTracker(&bb);
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::Instruction& inst) const{
	return getSlotTracker(&inst);
}

SlotTracker* SlotTrackerPass::getSlotTracker (const llvm::Argument& arg) const{
	return getSlotTracker(&arg);
}

char SlotTrackerPass::ID;
static RegisterPass<SlotTrackerPass>
X("slot-tracker", "Provides slot tracker functionality for other passes");

} // namespace borealis