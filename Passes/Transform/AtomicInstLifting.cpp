//
// Created by abdullin on 3/14/16.
//

#include "Util/util.h"
#include "Codegen/llvm.h"
#include "Util/passes.hpp"
#include "Util/functional.hpp"
#include "AtomicInstLifting.h"

#include "Util/macros.h"

namespace borealis {

AtomicInstLifting::AtomicInstLifting() : llvm::FunctionPass(ID) { }

bool AtomicInstLifting::runOnFunction(llvm::Function& F) {
    // Clear stuff in case we've been invoked several times in a row
    deleted_instructions.clear();

    if (not F.isDeclaration()) {
        for (auto &&I : util::viewContainer(F)
                .flatten()
                .map(ops::take_pointer)
                .map(llvm::dyn_caster<llvm::AtomicRMWInst>())
                .filter()) {
            processAtomicRMWInst(*I);
        }

        for (auto &&I : util::viewContainer(F)
                .flatten()
                .map(ops::take_pointer)
                .map(llvm::dyn_caster<llvm::AtomicCmpXchgInst>())
                .filter()) {
            processAtomicCmpXchgInst(*I);
        }

        for (auto&& i : util::viewContainer(deleted_instructions)
                .map(llvm::dyn_caster<llvm::Instruction>())
                .filter()) {
            for (auto&& user: util::viewContainer(i->users()).toVector()) {
                if (auto&& use = llvm::dyn_cast<llvm::Instruction>(user)) {
                    use->eraseFromParent();
                }
            }
            i->eraseFromParent();
        }
    }
    return true;
}

void AtomicInstLifting::processAtomicRMWInst(llvm::AtomicRMWInst& i) {
    auto&& ptr = i.getPointerOperand();
    auto&& val = i.getValOperand();

    if (i.getOperation() == atomicOps::Xchg) {
        auto* store = new llvm::StoreInst(val, ptr);
        copyMetadata(i, {store});
        store->insertBefore(&i);
        i.replaceAllUsesWith(val);

    } else if (isBinary(i.getOperation())) {
        auto* load = new llvm::LoadInst(ptr);
        auto* bin = llvm::BinaryOperator::Create(fromAtomicToBinary(i.getOperation()), load, val);
        auto* store = new llvm::StoreInst(bin, ptr);

        copyMetadata(i, {load, bin, store});
        load->insertBefore(&i);
        bin->insertBefore(&i);
        store->insertBefore(&i);
        i.replaceAllUsesWith(bin);

    } else if (i.getOperation() == atomicOps::Nand) {
        auto* load = new llvm::LoadInst(ptr);
        auto* bin = llvm::BinaryOperator::Create(llvmOps::And, load, val);
        ASSERTC(llvm::isa<llvm::IntegerType>(bin->getType()));
        auto allOnes = ~llvm::APInt::getNullValue(bin->getType()->getIntegerBitWidth());

        auto* notBin = llvm::BinaryOperator::Create(
                llvmOps::Xor,
                bin,
                llvm::ConstantInt::get(i.getContext(), allOnes)
        );
        auto* store = new llvm::StoreInst(notBin, ptr);

        copyMetadata(i, {load, bin, notBin, store});
        load->insertBefore(&i);
        bin->insertBefore(&i);
        notBin->insertBefore(&i);
        store->insertBefore(&i);
        i.replaceAllUsesWith(notBin);

    } else if (isCmp(i.getOperation())) {
        auto* load = new llvm::LoadInst(ptr);
        ASSERTC(llvm::isa<llvm::IntegerType>(load->getType()));
        auto* cmp = llvm::CmpInst::Create(llvm::CmpInst::OtherOps::ICmp, fromAtomicToCmp(i.getOperation()), load, val);
        auto* select = llvm::SelectInst::Create(cmp, load, val);
        auto* store = new llvm::StoreInst(select, ptr);

        copyMetadata(i, {load, cmp, select, store});
        load->insertBefore(&i);
        cmp->insertBefore(&i);
        select->insertBefore(&i);
        store->insertBefore(&i);
        i.replaceAllUsesWith(select);
    } else {
        errs() << "Unsupported type of operation in atomicrmw inst:" << i << endl;
    }

    deleted_instructions.insert(&i);
}

void AtomicInstLifting::processAtomicCmpXchgInst(llvm::AtomicCmpXchgInst& i) {
    auto&& ptr = i.getPointerOperand();
    auto&& cmpVal = i.getCompareOperand();
    auto&& val = i.getNewValOperand();

    auto* load = new llvm::LoadInst(ptr);
    auto* cmpInst = llvm::CmpInst::Create(llvm::CmpInst::OtherOps::ICmp, llvm::CmpInst::Predicate::ICMP_EQ, load, cmpVal);
    auto* select = llvm::SelectInst::Create(cmpInst, val, load);
    auto* store = new llvm::StoreInst(select, ptr);

    copyMetadata(i, {load, cmpInst, select, store});
    load->insertBefore(&i);
    cmpInst->insertBefore(&i);
    select->insertBefore(&i);
    store->insertBefore(&i);

    for (auto&& it: i.users()) {
        if (auto&& extract = llvm::dyn_cast<llvm::ExtractValueInst>(it)) {
            if (extract->getIndices()[0] == 0) extract->replaceAllUsesWith(select);
            else extract->replaceAllUsesWith(cmpInst);
        } else {
            errs() << "Unexpected user of cmpxchg inst: " << *it << endl;
        }
    }

    deleted_instructions.insert(&i);
}

bool AtomicInstLifting::isBinary(AtomicInstLifting::atomicOps op) {
    return op == atomicOps::Add ||
           op == atomicOps::Sub ||
           op == atomicOps::Or  ||
           op == atomicOps::Xor ||
           op == atomicOps::And;
}

AtomicInstLifting::llvmOps AtomicInstLifting::fromAtomicToBinary(AtomicInstLifting::atomicOps op) {
    if      (op == atomicOps::Add)  return llvmOps::Add;
    else if (op == atomicOps::Sub)  return llvmOps::Sub;
    else if (op == atomicOps::Or)   return llvmOps::Or;
    else if (op == atomicOps::Xor)  return llvmOps::Xor;
    else                            return llvmOps::And;
}

bool AtomicInstLifting::isCmp(AtomicInstLifting::atomicOps op) {
    return op == atomicOps::Min  ||
           op == atomicOps::Max  ||
           op == atomicOps::UMax ||
           op == atomicOps::UMin;
}

AtomicInstLifting::cmpOps AtomicInstLifting::fromAtomicToCmp(AtomicInstLifting::atomicOps op) {
    if      (op == atomicOps::Min)  return cmpOps::ICMP_SLT;
    else if (op == atomicOps::Max)  return cmpOps::ICMP_SGT;
    else if (op == atomicOps::UMax) return cmpOps::ICMP_UGT;
    else                            return cmpOps::ICMP_ULT;
}

void AtomicInstLifting::copyMetadata(const llvm::Instruction &i, const std::vector<llvm::Instruction*>& newInsts) {
    llvm::SmallVector<std::pair<unsigned, llvm::MDNode*>, 0> metadata;
    i.getAllMetadata(metadata);

    for (auto&& it: metadata) {
        for (auto&& newInst: newInsts) {
            newInst->setMetadata(it.first, it.second);
        }
    }
}

char AtomicInstLifting::ID = 0;
static llvm::RegisterPass<AtomicInstLifting>
X("atomic-lifting", "Pass that replaces atomic instructions with non-atomic equivalents", false, false);

}   /* namespace borealis */

#include "Util/unmacros.h"
