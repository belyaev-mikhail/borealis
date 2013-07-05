/*
 * LoopDeroll.cpp
 *
 *  Created on: Dec 7, 2012
 *      Author: ice-phoenix
 */

#include <llvm/Analysis/Dominators.h>
#include <llvm/Analysis/LoopIterator.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/Metadata.h>
#include <llvm/Support/CommandLine.h>
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include <vector>

#include "Passes/Transform/LoopDeroll.h"
#include "Util/passes.hpp"
#include "Util/util.h"

#include "Util/macros.h"

namespace borealis {

static llvm::cl::opt<unsigned>
DerollCount("deroll-count", llvm::cl::init(3), llvm::cl::NotHidden,
  llvm::cl::desc("Set loop derolling count (default = 3)"));

LoopDeroll::LoopDeroll() : llvm::LoopPass(ID) {}

static inline void RemapInstruction(
        llvm::Instruction& I,
        llvm::ValueToValueMapTy& VMap) {

    using namespace llvm;

    for (unsigned op = 0, e = I.getNumOperands(); op != e; ++op) {
        Value* Op = I.getOperand(op);
        ValueToValueMapTy::iterator It = VMap.find(Op);
        if (It != VMap.end()) {
            I.setOperand(op, It->second);
        // Attempt to remap internal MDNode values to cloned ones
        // to preserve debug information when unrolling
        } else if (auto* MDN = dyn_cast_or_null<MDNode>(Op)) {

            std::vector<Value*> mdn_values;
            mdn_values.reserve(MDN->getNumOperands());
            bool hasClonedValues = false;

            for (unsigned mdn_op = 0, ee = MDN->getNumOperands(); mdn_op != ee; ++mdn_op) {
                Instruction* OldVal = dyn_cast_or_null<Instruction>(MDN->getOperand(mdn_op));
                ValueToValueMapTy::iterator Mdn_It = VMap.find(OldVal);
                if (OldVal && Mdn_It != VMap.end()) {
                    mdn_values.push_back(Mdn_It->second);
                    hasClonedValues = true;
                } else {
                    mdn_values.push_back(OldVal);
                }
            }

            if (hasClonedValues) {
                I.setOperand(op, MDNode::get(MDN->getContext(), mdn_values));
            }
        }
    }

    if (isa<PHINode>(I)) {
        PHINode& PN = cast<PHINode>(I);
        for (unsigned i = 0, e = PN.getNumIncomingValues(); i != e; ++i) {
            ValueToValueMapTy::iterator It = VMap.find(PN.getIncomingBlock(i));
            if (It != VMap.end()) {
                PN.setIncomingBlock(i, cast<BasicBlock>(It->second));
            }
        }
    }
}

static inline llvm::BasicBlock* CreateUnreachableBasicBlock(
        llvm::Function* F,
        llvm::Twine Name) {

    using namespace llvm;

    BasicBlock* BB = BasicBlock::Create(
            F->getContext(),
            Name + Twine(".unreachable"),
            F
    );

    new UnreachableInst(F->getContext(), BB);

    return BB;
}

void LoopDeroll::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    using namespace llvm;

    AUX<LoopInfo>::addRequiredTransitive(AU);
    AUX<LoopManager>::addRequiredTransitive(AU);
    AUX<ScalarEvolution>::addRequiredTransitive(AU);
    // Does NOT preserve CFG
}

bool LoopDeroll::runOnLoop(llvm::Loop* L, llvm::LPPassManager& LPM) {
    using namespace llvm;

    // Basic info about the loop
    BasicBlock* Header = L->getHeader();
    BasicBlock* Latch = L->getLoopLatch();
    BasicBlock* LoopPredecessor = L->getLoopPredecessor();

    // Sanity checks
    if (Header == nullptr) {
        BYE_BYE(bool, "Cannot unroll a loop with multiple headers");
    }
    if (Latch == nullptr) {
        BYE_BYE(bool, "Cannot unroll a loop with multiple latches");
    }
    if (LoopPredecessor == nullptr) {
        BYE_BYE(bool, "Cannot unroll a loop with multiple predecessors");
    }

    Function* F = Header->getParent();

    LI = &getAnalysis<LoopInfo>();
    LM = &getAnalysis<LoopManager>();
    SE = &getAnalysis<ScalarEvolution>();

    // Loop exit info
    BranchInst* BI = cast<BranchInst>(Latch->getTerminator());
    bool ContinueOnTrue = L->contains(BI->getSuccessor(0));

    BasicBlock* LoopExit;
    if (BI->getNumSuccessors() == 1) {
        LoopExit = nullptr;
    } else {
        LoopExit = BI->getSuccessor(ContinueOnTrue);
    }

    // Cloning bookkeeping
    std::vector<BasicBlock*> Headers;
    std::vector<BasicBlock*> Latches;
    Headers.push_back(Header);
    Latches.push_back(Latch);

    // Value map for the last cloned iteration
    ValueToValueMapTy LastValueMap;

    // Original loop PHI nodes needed for the first iteration
    std::vector<PHINode*> OrigPHINodes;
    for (auto& I : *Header) {
        if (isa<PHINode>(I)) {
            OrigPHINodes.push_back(&cast<PHINode>(I));
        } else {
            break;
        }
    }

    // LLVM on-the-fly SSA update requires blocks to be processed in
    // reverse post-order, so that LastValueMap contains the correct value
    // at each exit
    LoopBlocksDFS DFS(L);
    DFS.perform(LI);
    // Stash the DFS iterators before adding blocks to the loop
    LoopBlocksDFS::RPOIterator BlockBegin = DFS.beginRPO();
    LoopBlocksDFS::RPOIterator BlockEnd = DFS.endRPO();

    unsigned CurrentDerollCount = DerollCount;

    // Try to guess the deroll count
    unsigned TripCount = SE->getSmallConstantTripCount(L, Latch);
    if (TripCount != 0) CurrentDerollCount = TripCount;

    // Try to find unroll annotation for this loop
    unsigned AnnoUnrollCount = LM->getUnrollCount(L);
    if (AnnoUnrollCount != 0) CurrentDerollCount = AnnoUnrollCount;

    for (unsigned UnrollIter = 0; UnrollIter != CurrentDerollCount; UnrollIter++) {
        std::vector<BasicBlock*> NewBlocks;

        for (auto BB = BlockBegin; BB != BlockEnd; ++BB) {
            ValueToValueMapTy VMap;
            BasicBlock* New = CloneBasicBlock(*BB, VMap, ".bor." + Twine(UnrollIter));
            F->getBasicBlockList().push_back(New);
            L->addBasicBlockToLoop(New, LI->getBase());

            // Loop over all of the PHI nodes in the block, changing them to use
            // the incoming values from the previous block
            if (Header == *BB) {
                for (auto* OrigPHINode : OrigPHINodes) {
                    PHINode* NewPHI = cast<PHINode>(VMap[OrigPHINode]);
                    Value* InVal = NewPHI->getIncomingValueForBlock(Latch);
                    if (Instruction* InValI = dyn_cast<Instruction>(InVal)) {
                        if (UnrollIter > 0 && L->contains(InValI)) {
                            InVal = LastValueMap[InValI];
                        }
                    }
                    VMap[OrigPHINode] = InVal;
                    New->getInstList().erase(NewPHI);
                }
            }

            // Update the running map of newest clones
            LastValueMap[*BB] = New;
            for (const auto& V : VMap) {
                LastValueMap[V.first] = V.second;
            }

            // Add PHI entries for newly created values to all exit blocks
            for (auto SI = succ_begin(*BB), SE = succ_end(*BB); SI != SE; ++SI) {
                if (L->contains(*SI))
                    continue;
                for (auto BBI = (*SI)->begin(); PHINode* PHI = dyn_cast<PHINode>(BBI); ++BBI) {
                    Value* Incoming = PHI->getIncomingValueForBlock(*BB);
                    ValueToValueMapTy::iterator It = LastValueMap.find(Incoming);
                    if (It != LastValueMap.end())
                        Incoming = It->second;
                    PHI->addIncoming(Incoming, New);
                }
            }

            if (Header == *BB) Headers.push_back(New);
            if (Latch == *BB) Latches.push_back(New);
            NewBlocks.push_back(New);
        }

        // Remap all instructions in the most recent iteration
        for (auto* BB : NewBlocks) {
            for (auto& I : *BB) {
                RemapInstruction(I, LastValueMap);
            }
        }
    }

    // Loop over the PHI nodes in the original block, setting incoming values
    for (auto* PN : OrigPHINodes) {
        PN->replaceAllUsesWith(PN->getIncomingValueForBlock(LoopPredecessor));
        Header->getInstList().erase(PN);
    }

    // Now that all the basic blocks for the unrolled iterations are in place,
    // set up the branches to connect them
    for (unsigned LL = 0, E = Latches.size(); LL != E; ++LL) {
        // The original branch was replicated in each unrolled iteration
        BranchInst* Term = cast<BranchInst>(Latches[LL]->getTerminator());

        // The branch destination
        unsigned DD = (LL + 1) % E;
        BasicBlock* Dest = Headers[DD];

        if (DD != 0) {
            Term->setSuccessor(!ContinueOnTrue, Dest);
        } else if (LoopExit) {
            Term->setSuccessor(!ContinueOnTrue, LoopExit);
            BasicBlock* UBB = CreateUnreachableBasicBlock(F, Twine(Latches[LL]->getName()));
            Term->setSuccessor(ContinueOnTrue, UBB);
        } else {
            new UnreachableInst(Term->getContext(), Term);
            Term->eraseFromParent();
        }
    }

    // Reconstruct dom info, because it is not preserved properly
    if (DominatorTree* DT = LPM.getAnalysisIfAvailable<DominatorTree>()) {
        DT->runOnFunction(*F);
    }

    // This loop shall be no more...
    LPM.deleteLoopFromQueue(L);

    return true;
}

char LoopDeroll::ID;
static RegisterPass<LoopDeroll>
X("loop-deroll", "Loop de-roller pass");

} /* namespace borealis */

#include "Util/unmacros.h"