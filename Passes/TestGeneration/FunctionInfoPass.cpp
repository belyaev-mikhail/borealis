/* 
 * File:   FunctionInfoPass.cpp
 * Author: maxim
 * 
 * Created on 19 Май 2014 г., 13:29
 */

#include "Passes/TestGeneration/FunctionInfoPass.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "Passes/TestGeneration/TestGenerationPass.h"

namespace borealis {

FunctionInfoPass::FunctionInfoPass() : ModulePass(ID) {}

void FunctionInfoPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();

    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
    AUX<TestGenerationPass>::addRequiredTransitive(AU);
    AUX<TestManager>::addRequiredTransitive(AU);
}

bool FunctionInfoPass::runOnModule(llvm::Module& M) {
    util::use(M);
    
    tm = &GetAnalysis<TestManager>::doit(this);
    auto * stp = &GetAnalysis<SlotTrackerPass>::doit(this);
    
    for (const auto* f: tm->getFunctions()) {
        auto* st = stp->getSlotTracker(f);
        FunctionInfo fi(f, st);
        functions.emplace(f, fi);
    }
    
    return true;
}

FunctionInfoPass::~FunctionInfoPass() {}

FunctionInfo FunctionInfoPass::getFunctionInfo(const llvm::Function* F) const {
    return functions.at(F);
}

char FunctionInfoPass::ID;
static RegisterPass<FunctionInfoPass>
X("function-info", "Pass that find metainfo for functions.");

} /* namespace borealis */

