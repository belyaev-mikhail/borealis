//
// Created by stepanov on 10/31/16.
//

#ifndef BOREALIS_FUNANALYZECHOICER_H
#define BOREALIS_FUNANALYZECHOICER_H


#include <llvm/IR/DebugInfo.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Pass.h>



namespace borealis {

class FunAnalyzeChoicer : public llvm::ModulePass {

public:

    static char ID;

    FunAnalyzeChoicer() : llvm::ModulePass(ID) {}
    virtual bool runOnModule(llvm::Module& M) override;

    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;

private:

    CallGraphChopper* CGC;
};
} //namespace borealis
#endif //BOREALIS_FUNANALYZECHOICER_H
