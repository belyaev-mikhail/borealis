/*
 * PredicateStateAnalysis.cpp
 *
 *  Created on: Sep 4, 2012
 *      Author: ice-phoenix
 */

#include "PredicateStateAnalysis.h"

#include <llvm/BasicBlock.h>
#include <llvm/Constants.h>
#include <llvm/Support/raw_ostream.h>

namespace borealis {

using util::contains;
using util::containsKey;
using util::for_each;
using util::streams::endl;
using util::toString;

typedef PredicateAnalysis::PredicateMap PM;
typedef PredicateAnalysis::TerminatorPredicateMap TPM;

PredicateStateAnalysis::PredicateStateAnalysis() : llvm::FunctionPass(ID) {
    // TODO
}

void PredicateStateAnalysis::getAnalysisUsage(llvm::AnalysisUsage& Info) const{
    using namespace::llvm;

    Info.setPreservesAll();
    Info.addRequiredTransitive<PredicateAnalysis>();
}

bool PredicateStateAnalysis::runOnFunction(llvm::Function& F) {
    using namespace::std;
    using namespace::llvm;

    PA = &getAnalysis<PredicateAnalysis>();

    workQueue.push(make_pair(&F.getEntryBlock(), PredicateStateVector(true)));
    processQueue();

    errs() << endl << "PSA results:" << endl;
    for_each(F, [this](const BasicBlock& BB) {
        for_each(BB, [this](const Instruction& I) {
            if (containsKey(this->predicateStateMap, &I)) {
                auto vec = predicateStateMap[&I];
                errs() << I << endl;
                errs() << vec << endl;
            }
        });
    });
    errs() << endl << "End of PSA results" << endl;

    return false;
}

void PredicateStateAnalysis::processQueue() {
    while (!workQueue.empty()) {
        processBasicBlock(workQueue.front());
        workQueue.pop();
    }
}

void PredicateStateAnalysis::processBasicBlock(const WorkQueueEntry& wqe) {
    using namespace::std;
    using namespace::llvm;

    PM& pm = PA->getPredicateMap();

    const BasicBlock& bb = *(wqe.first);
    PredicateStateVector inStateVec = wqe.second;
    bool shouldScheduleTerminator = true;

    errs() << &bb << " <- " << endl << inStateVec << endl;

    for (auto inst = bb.begin(); inst != bb.end(); ++inst) {
        const Instruction& I = *inst;
        PredicateStateVector stateVec;

        bool hasPredicate = containsKey(pm, &I);
        bool hasState = containsKey(predicateStateMap, &I);

        if (!hasPredicate) continue;
        if (hasState) stateVec = predicateStateMap[&I];

        PredicateStateVector modifiedInStateVec =
                inStateVec.addPredicate(pm[&I]);
        PredicateStateVector merged =
                stateVec.merge(modifiedInStateVec);

        if (stateVec == merged) {
            shouldScheduleTerminator = false;
            break;
        }

        predicateStateMap[&I] = inStateVec = merged;
    }

    errs() << &bb << " -> " << endl << inStateVec << endl;

    if (shouldScheduleTerminator) {
        processTerminator(*bb.getTerminator(), inStateVec);
    }
}

void PredicateStateAnalysis::processTerminator(
        const llvm::TerminatorInst& I,
        const PredicateStateVector& state) {
    using namespace::llvm;

    if (isa<BranchInst>(I))
    { process(cast<BranchInst>(I), state); }
}

void PredicateStateAnalysis::process(
        const llvm::BranchInst& I,
        const PredicateStateVector& state) {
    using namespace::std;
    using namespace::llvm;

    TPM& tpm = PA->getTerminatorPredicateMap();

    if (I.isUnconditional()) {
        const BasicBlock* succ = I.getSuccessor(0);
        workQueue.push(make_pair(succ, state));
    } else {
        const BasicBlock* trueSucc = I.getSuccessor(0);
        const BasicBlock* falseSucc = I.getSuccessor(1);

        const Predicate* truePred = tpm[make_pair(&I, trueSucc)];
        const Predicate* falsePred = tpm[make_pair(&I, falseSucc)];

        workQueue.push(make_pair(trueSucc, state.addPredicate(truePred)));
        workQueue.push(make_pair(falseSucc, state.addPredicate(falsePred)));
    }
}

PredicateStateAnalysis::~PredicateStateAnalysis() {
    // TODO
}

} /* namespace borealis */

char borealis::PredicateStateAnalysis::ID;
static llvm::RegisterPass<borealis::PredicateStateAnalysis>
X("predicate-state", "Predicate state analysis", false, false);
