/* 
 * File:   CUnitDumperPass.cpp
 * Author: maxim
 * 
 * Created on 18 Февраль 2014 г., 17:16
 */

#include "CUnitDumperPass.h"

namespace borealis {

CUnitDumperPass::CUnitDumperPass() : ModulePass(ID) {}

void CUnitDumperPass::getAnalysisUsage(llvm::AnalysisUsage & AU) const {
    AU.setPreservesAll();

    AUX<FunctionManager>::addRequiredTransitive(AU);
    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
    AUX<TestGenerationPass>::addRequiredTransitive(AU);
}

bool CUnitDumperPass::runOnModule(llvm::Module & M) {
    testFile.open("test.c", std::ios::out);
    generateHeader();
    
    for (auto & f: M) {
        auto * fm = &GetAnalysis<FunctionManager>::doit(this, f);
        auto * st = GetAnalysis<SlotTrackerPass>::doit(this, f).getSlotTracker(f);
        auto fn = FactoryNest(st);
        
        auto testSuite = fm->getTests(&f);
        testSuite->generateTest(testFile, fn);
    }
    
    testFile << "int main() {\n"
             << "    if (CUE_SUCCESS != CU_initialize_registry())\n"
             << "        return CU_get_error();\n";
    
    for (auto & f: M) {
        auto * fm = &GetAnalysis<FunctionManager>::doit(this, f);
        auto * st = GetAnalysis<SlotTrackerPass>::doit(this, f).getSlotTracker(f);
        auto fn = FactoryNest(st);
        
        auto testSuite = fm->getTests(&f);
        testSuite->activateTest(testFile);
    }
    
    testFile << "    CU_basic_set_mode(CU_BRM_VERBOSE);\n"
             << "    CU_basic_run_tests();\n"
             << "    CU_cleanup_registry();\n"
             << "    return CU_get_error();\n"
             << "}\n";
    
    testFile.close();
    
    return true;
}

CUnitDumperPass::~CUnitDumperPass() {}

void CUnitDumperPass::generateHeader() {
    testFile << "#include <CUnit/Basic.h>\n\n";
}


char CUnitDumperPass::ID;
static RegisterPass<CUnitDumperPass>
X("cunit-dumper", "Pass that dumps unit tests into CUnit tests.");

} /* namespace borealis */