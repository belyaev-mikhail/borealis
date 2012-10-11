/*
 * PredicateStateAnalysis.h
 *
 *  Created on: Sep 4, 2012
 *      Author: ice-phoenix
 */

#ifndef PREDICATESTATEANALYSIS_H_
#define PREDICATESTATEANALYSIS_H_

#include <llvm/Function.h>
#include <llvm/Instructions.h>
#include <llvm/Pass.h>

#include <map>
#include <queue>
#include <set>
#include <vector>

#include "../State/PredicateState.h"
#include "../State/PredicateStateVector.h"
#include "PredicateAnalysis.h"

namespace borealis {

class PredicateStateAnalysis: public llvm::FunctionPass {

public:

    typedef std::map<const llvm::Instruction*, PredicateStateVector> PredicateStateMap;
    typedef std::pair<const llvm::Instruction*, PredicateStateVector> PredicateStateMapEntry;

    typedef std::pair<const llvm::BasicBlock*, PredicateStateVector> WorkQueueEntry;
    typedef std::queue<WorkQueueEntry> WorkQueue;

    static char ID;

    PredicateStateAnalysis();
    virtual bool runOnFunction(llvm::Function& F);
    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const;
    virtual ~PredicateStateAnalysis();

    PredicateStateMap& getPredicateStateMap() {
        return predicateStateMap;
    }

private:

    PredicateStateMap predicateStateMap;
    WorkQueue workQueue;

    PredicateAnalysis* PA;

    void processQueue();
    void processBasicBlock(const WorkQueueEntry& wqe);
    void processTerminator(const llvm::TerminatorInst& I, const PredicateStateVector& state);
    void process(const llvm::BranchInst& I, const PredicateStateVector& state);

};

} /* namespace borealis */

#endif /* PREDICATESTATEANALYSIS_H_ */
