/* 
 * File:   FunctionInfoPass.cpp
 * Author: maxim
 * 
 * Created on 19 Май 2014 г., 13:29
 */

#include "Passes/TestGeneration/FunctionInfoPass.h"
#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/Tracker/SlotTrackerPass.h"

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
    
    std::unordered_map<std::string, FunctionInfo*> stubFuncs;
    std::vector<FunctionInfo*> fakeFuncs;
    
    for (const auto* f: tm->getFunctions()) {
        auto* st = stp->getSlotTracker(f);
        FunctionInfo fi(f, st);
        auto& fiInst = functions.emplace(f, fi).first->second;

        if (fiInst.isStub()) {
            stubFuncs.emplace("__" + fiInst.getRealName().str(), &fiInst);
        }
        
        if (fiInst.isFake()) {
            fakeFuncs.push_back(&fiInst);
        }
    }
    
    for (auto sF: fakeFuncs) {
        sF->setStubFunc(stubFuncs.at(sF->getRealName().str()));
    }
    
    tm->setSuitesFunctionInfo(*this);
    
    return true;
}

FunctionInfoPass::~FunctionInfoPass() {}

const FunctionInfo& FunctionInfoPass::getFunctionInfo(const llvm::Function* F) const {
    return functions.at(F);
}

char FunctionInfoPass::ID;
static RegisterPass<FunctionInfoPass>
X("function-info", "Pass that find metainfo for functions.");

} /* namespace borealis */

