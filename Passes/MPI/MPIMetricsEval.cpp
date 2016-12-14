//
// Created by stepanov on 10/28/16.
//

#include "MPIMetricsEval.h"
#include "Codegen/llvm.h"
#include "Util/passes.hpp"

#include <llvm/Analysis/LoopPass.h>
#include <llvm/Support/Debug.h>

namespace borealis{

void MPIMetricsEval::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
}

bool MPIMetricsEval::runOnFunction(llvm::Function& F){
    float koefSize = -0.02;
    float koefInst = 0.13;
    int size = 0;
    int loads=0, stores = 0, gep = 0;
    for(auto&& bb : F)
        for(auto&& it : bb) {
            ++size;
            if (llvm::isa<llvm::StoreInst>(it))
                ++stores;
            else if (llvm::isa<llvm::LoadInst>(it))
                ++loads;
            else if (llvm::isa<llvm::GetElementPtrInst>(it))
                ++gep;
        }
    difficult = size * koefSize + (loads + stores + gep) * koefInst;
    if(difficult <= 0) difficult = 0.001;
    return false;
}

void MPIMetricsEval::print(llvm::raw_ostream&, const llvm::Module*) const {
    auto info = errs();
    info << difficult;
}


char MPIMetricsEval::ID;
static RegisterPass<MPIMetricsEval>
        X("mpi-metrics-eval", "Pass that evaluates functions analyze metrics for distributing over processes");

} // namespace Borealis