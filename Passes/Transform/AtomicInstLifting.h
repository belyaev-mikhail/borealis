//
// Created by abdullin on 3/14/16.
//

#ifndef BOREALIS_ATOMICINSTLIFTING_H
#define BOREALIS_ATOMICINSTLIFTING_H

#include <llvm/Pass.h>
#include <llvm/IR/Instructions.h>

namespace borealis {

class AtomicInstLifting : public llvm::FunctionPass {

    using trash_set = std::unordered_set<llvm::Value*>;

public:

    using llvmOps = llvm::Instruction::BinaryOps;
    using cmpOps = llvm::CmpInst::Predicate;
    using atomicOps = llvm::AtomicRMWInst::BinOp;

    static char ID;

    AtomicInstLifting();
    virtual ~AtomicInstLifting() = default;

    virtual bool runOnFunction(llvm::Function& F) override;

private:

    void processAtomicRMWInst(llvm::AtomicRMWInst& i);
    void processAtomicCmpXchgInst(llvm::AtomicCmpXchgInst& i);

    bool isBinary(atomicOps op);
    bool isSelect(atomicOps op);
    llvmOps fromBinaryAtomic(atomicOps op);
    cmpOps fromSelectAtomic(atomicOps op);

    void copyMetadata(llvm::Instruction& i, std::vector<llvm::Instruction*> newInsts);

    trash_set deleted_instructions;

};

} /* namespace borealis */

#endif //BOREALIS_ATOMICINSTLIFTING_H
