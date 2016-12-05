//
// Created by stepanov on 10/28/16.
//

#ifndef BOREALIS_MPIMETRICSEVAL_H
#define BOREALIS_MPIMETRICSEVAL_H

#include <llvm/IR/DebugInfo.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Pass.h>
#include <vector>

namespace borealis{

class MPIMetricsEval : public llvm::FunctionPass {

private:

    float difficult;

public:

    static char ID;

    float getDifficult(){return difficult;}

    MPIMetricsEval() : llvm::FunctionPass(ID) {}
    virtual bool runOnFunction(llvm::Function& F) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
    virtual void print(llvm::raw_ostream& O, const llvm::Module* M = nullptr) const override;
    virtual ~MPIMetricsEval() {}

};

} // namespace Borealis
#endif //BOREALIS_MPIMETRICSEVAL_H
