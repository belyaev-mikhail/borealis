/*
 * NameTracker.cpp
 *
 *  Created on: Oct 11, 2012
 *      Author: belyaev
 */

#include "Passes/NameTracker.h"
#include "Util/passes.hpp"

namespace borealis {

static void addName(llvm::Value& V, NameTracker::nameResolver_t& resolver) {
    if (V.hasName()) resolver[V.getName()] = &V;
}

bool NameTracker::runOnModule(llvm::Module& M) {
	for (auto& G : M.getGlobalList()) {
	    addName(G, globalResolver);
	}
	for (auto& F : M) {
	    addName(F, globalResolver);
		for (auto& A : F.getArgumentList()) {
		    addName(A, globalResolver);
		}
		for (auto& BB : F) {
			for (auto& I: BB) {
			    addName(I, globalResolver);
			}
		}
	}
	return false;
}

void NameTracker::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
	AU.setPreservesAll();
}

char NameTracker::ID = 0;
static RegisterPass<NameTracker>
X("name-tracker", "Pass used to track value names in LLVM IR");

} // namespace borealis
