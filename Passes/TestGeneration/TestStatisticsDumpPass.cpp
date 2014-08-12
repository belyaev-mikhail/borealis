/* 
 * File:   TestStatisticsDumpPass.cpp
 * Author: maxim
 * 
 * Created on 11 Август 2014 г., 15:11
 */

#include <iostream>

#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/TestGeneration/TestStatisticsDumpPass.h"
#include "Util/util.hpp"

namespace borealis {

TestStatisticsDumpPass::TestStatisticsDumpPass() : ModulePass(ID) {}

void TestStatisticsDumpPass::getAnalysisUsage(llvm::AnalysisUsage & AU) const {
    AU.setPreservesAll();

    AUX<FunctionInfoPass>::addRequiredTransitive(AU);
    AUX<TestGenerationPass>::addRequiredTransitive(AU);
    AUX<TestStatistics>::addRequiredTransitive(AU);
}

bool TestStatisticsDumpPass::runOnModule(llvm::Module & M) {
    util::use(M);
    
    stat = &GetAnalysis<TestStatistics>::doit(this);
    
    const auto& fip = GetAnalysis<FunctionInfoPass>::doit(this);
    
    stat->cleanFakeFunctions(fip);
    
    std::cout << "Test generation statistics:" << std::endl;
    
    std::cout << "\t" << stat->getTotalGood() << " tests generated" << std::endl;
    auto totalStubs = stat->getTotalStubs();
    if (totalStubs > 0) {
        std::cout << "\t" << totalStubs << " stubs generated" << std::endl;
    }
    
    std::cout << std::endl;
    
    /*auto totalUnknown = stat->getTotalUnknown();
    if (totalUnknown > 0) {
        std::cout << totalUnknown << " tests failed to generate" << std::endl;
    }*/
    
    std::cout << "Statistics by functions:" << std::endl;
    
    for (auto fs: *stat) {
        std::cout << "\t" << fs.first->getName() << ": " << fs.second.good << " tests";
        if (fs.second.stubs > 0) {
            std::cout << " with stubs";
        }
        /*if (fs.second.unknown > 0) {
            std::cout << ", " << fs.second.unknown << " tests failed to generate";
        }*/
        std::cout << std::endl;
    }
    
    return true;
}

TestStatisticsDumpPass::~TestStatisticsDumpPass() {}

char TestStatisticsDumpPass::ID;
static RegisterPass<TestStatisticsDumpPass>
X("test-statistics-dump", "Pass that dumps tests statistics.");

} /* namespace borealis */