/*
 * PredicateAnalysis.h
 *
 *  Created on: Aug 31, 2012
 *      Author: ice-phoenix
 */

#ifndef PREDICATEANALYSIS_H_
#define PREDICATEANALYSIS_H_

#include <llvm/Constants.h>
#include <llvm/Function.h>
#include <llvm/Instructions.h>
#include <llvm/Pass.h>
#include <llvm/Support/InstVisitor.h>
#include <llvm/Target/TargetData.h>

#include <map>
#include <set>
#include <vector>

#include "Predicate/PredicateFactory.h"

#include "slottracker.h"
#include "util.h"

namespace borealis {

class PredicateAnalysis: public llvm::FunctionPass {

public:

	typedef std::map<const llvm::Instruction*, Predicate*> PredicateMap;
	typedef std::pair<const llvm::Instruction*, Predicate*> PredicateMapEntry;

	typedef std::pair<const llvm::TerminatorInst*, const llvm::BasicBlock*> TerminatorBranch;
	typedef std::map<TerminatorBranch, Predicate*> TerminatorPredicateMap;
	typedef std::pair<TerminatorBranch, Predicate*> TerminatorPredicateMapEntry;

	static char ID;

	PredicateAnalysis();
	virtual bool runOnFunction(llvm::Function& F);
	virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const;
	virtual ~PredicateAnalysis();

	PredicateMap& getPredicateMap() {
		return PM;
	}

	TerminatorPredicateMap& getTerminatorPredicateMap() {
		return TPM;
	}

	void init() {
	    PM.clear();
	    TPM.clear();
	}

private:

	PredicateMap PM;
	TerminatorPredicateMap TPM;

	std::shared_ptr<PredicateFactory> PF;
	TargetData* TD;

};

} /* namespace borealis */

#endif /* PREDICATEANALYSIS_H_ */
