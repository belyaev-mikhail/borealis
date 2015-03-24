/*
 * GlobalVariableLifting.cpp
 *
 *  Created on: 23 марта 2015 г.
 *      Author: abdullin
 */

#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

#include <unordered_map>
#include <unordered_set>

#include "Passes/Transform/GlobalVariableLifting.h"
#include "Util/util.h"

namespace borealis {

GlobalVariableLifting::GlobalVariableLifting() : llvm::FunctionPass(ID) {}

void GlobalVariableLifting::init() {
    globals.clear();
    locals_begin.clear();
    locals_end.clear();
    deleted_instructions.clear();
}

void GlobalVariableLifting::collectGlobals(llvm::Function& F) {
    for (auto&& inst = llvm::inst_begin(F); inst != llvm::inst_end(F); ++inst) {
        for (auto&& op : inst->operand_values()) {
            if (llvm::isa<llvm::GlobalVariable>(op) &&
                not op->getType()->getPointerElementType()->isArrayTy())
                    globals[op->getName()] = op;
        }
    }
}

void GlobalVariableLifting::setupBasicBlock(llvm::BasicBlock* BB) {
    auto&& bb_locals_begin = locals_begin[BB];
    auto&& bb_locals_end = locals_end[BB];

    auto&& inst = BB->begin();

    // insert loads at the start of BB
    for (auto&& g : globals) {
        auto&& gName = g.first();
        auto&& gValue = g.second;
        auto* load = new llvm::LoadInst(gValue, gName + LIFTED_POSTFIX, inst);
        bb_locals_begin[gName] = bb_locals_end[gName] = load;
    }

    for ( ; inst != BB->end(); ++inst) {
        // replace globals to locals in loads/stores
        if (auto* load = llvm::dyn_cast<llvm::LoadInst>(inst)) {
            if (auto* gv = llvm::dyn_cast<llvm::GlobalVariable>(load->getPointerOperand())) {
                load->replaceAllUsesWith(bb_locals_end[gv->getName()]);
                deleted_instructions.insert(load);
            }
        } else if (auto* store = llvm::dyn_cast<llvm::StoreInst>(inst)) {
            if (auto* gv = llvm::dyn_cast<llvm::GlobalVariable>(store->getPointerOperand())) {
                bb_locals_end[gv->getName()] = store->getValueOperand();
                deleted_instructions.insert(store);
            }
        }
        // store everything before terminators
        else if (auto* terminator = llvm::dyn_cast<llvm::TerminatorInst>(inst)) {
            if (llvm::Instruction::Br != terminator->getOpcode()) {
                for (auto&& g : globals) {
                    auto&& gName = g.first();
                    auto&& gValue = g.second;
                    new llvm::StoreInst(bb_locals_end[gName], gValue, terminator);
                }
            }
        // do store/load around calls to other functions
        } else if (auto* call = llvm::dyn_cast<llvm::CallInst>(inst)) {
            if (call->getCalledFunction()->doesNotAccessMemory()) continue;

            auto&& next_inst = std::next(inst);
            for (auto&& g : globals) {
                auto&& gName = g.first();
                auto&& gValue = g.second;
                new llvm::StoreInst(bb_locals_end[gName], gValue, inst);
                bb_locals_end[gName] = new llvm::LoadInst(gValue, gName + LIFTED_POSTFIX, next_inst);
            }
            inst = --next_inst;
        }
    }
}

void GlobalVariableLifting::linkBasicBlock(llvm::BasicBlock* BB) {
    auto&& bb_locals_begin = locals_begin[BB];
    auto&& bb_locals_end = locals_end[BB];

    // remove unused locals in BB with one predecessor
    if (auto* pred = BB->getUniquePredecessor()) {
        for (auto&& g : globals) {
            auto&& gName = g.first();
            auto&& pred_value = locals_end[pred][gName];
            if (bb_locals_begin[gName] == bb_locals_end[gName]) {
                bb_locals_end[gName] = pred_value;
            }
            bb_locals_begin[gName]->replaceAllUsesWith(pred_value);
            deleted_instructions.insert(bb_locals_begin[gName]);
            bb_locals_begin[gName] = pred_value;
        }
    // add phi nodes in BB with several predecessors
    } else {
        auto&& pred_num = std::distance(llvm::pred_begin(BB), llvm::pred_end(BB));
        for (auto&& g : globals) {
            auto&& gName = g.first();
            auto* phi = llvm::PHINode::Create(
                    bb_locals_end[gName]->getType(),
                    pred_num,
                    gName + LIFTED_POSTFIX,
                    &BB->front()
            );
            for (auto&& pred = llvm::pred_begin(BB); pred != llvm::pred_end(BB); ++pred) {
                phi->addIncoming(locals_end[*pred][gName], *pred);
            }
            if (bb_locals_begin[gName] == bb_locals_end[gName]) {
                bb_locals_end[gName] = phi;
            }
            bb_locals_begin[gName]->replaceAllUsesWith(phi);
            deleted_instructions.insert(bb_locals_begin[gName]);
            bb_locals_begin[gName] = phi;
        }
    }
}

bool GlobalVariableLifting::runOnFunction(llvm::Function& F) {
    using namespace llvm;

    init();

    collectGlobals(F);

    for (auto&& bb = F.begin(); bb != F.end(); ++bb) {
        setupBasicBlock(bb);
    }

    for (auto&& bb = ++F.begin(); bb != F.end(); ++bb) {
        linkBasicBlock(bb);
    }

    // deleting unused instructions
    for (auto&& i : util::viewContainer(deleted_instructions)
                    .map(llvm::dyn_caster<llvm::Instruction>())
                    .filter()) {
        i->eraseFromParent();
    }

    return true;
}

char GlobalVariableLifting::ID = 0;

static llvm::RegisterPass<GlobalVariableLifting>
X("gvl", "global variable lifting pass", false, false);

}  /* namespace borealis */
