/* 
 * File:   CUnitDumperPass.cpp
 * Author: maxim
 * 
 * Created on 18 Февраль 2014 г., 17:16
 */

#include "Passes/TestGeneration/CUnitDumperPass.h"

#include "Passes/Defect/DefectManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/TestGeneration/TestManager.h"
#include "Passes/Tracker/MetaInfoTracker.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "Passes/Tracker/FunctionAnnotationTracker.h"

#include "State/Transformer/ContractStupidifier.h"

namespace borealis {

CUnitDumperPass::CUnitDumperPass() : ModulePass(ID) {}

void CUnitDumperPass::getAnalysisUsage(llvm::AnalysisUsage & AU) const {
    AU.setPreservesAll();

    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
    AUX<MetaInfoTracker>::addRequiredTransitive(AU);
    AUX<TestGenerationPass>::addRequiredTransitive(AU);
    AUX<TestManager>::addRequiredTransitive(AU);
    AUX<FunctionAnnotationTracker>::addRequiredTransitive(AU);
}

bool CUnitDumperPass::runOnModule(llvm::Module & M) {
    testFile.open("test.c", std::ios::out);
    generateHeader();
    
    auto * tm = &GetAnalysis<TestManager>::doit(this);
    auto * stp = &GetAnalysis<SlotTrackerPass>::doit(this);
    auto * mit = &GetAnalysis<MetaInfoTracker>::doit(this);

    FunctionAnnotationTracker& FAT = GetAnalysis<FunctionAnnotationTracker>::doit(this);
    
    for (auto & f: M) {
        auto testSuite = tm->getTests(&f);
        if (testSuite != nullptr) {
            testSuite->prototypeFunction(testFile, mit);
        }
    }
    
    testFile << "\n";
    


    for (auto & f: M) {
        auto * st = stp->getSlotTracker(f);
        auto fn = FactoryNest(st);
        
        auto testSuite = tm->getTests(&f);

        if (testSuite != nullptr) {
            ContractStupidifier cs{
                util::view(f.arg_begin(), f.arg_end())
                     .map([&](llvm::Argument& a){
                          return fn.Term->getArgumentTerm(&a);
                      })
                     .toVector(),
                fn.Term->getValueTerm(fn.Type->getUnknownType(), "res"),
                fn
            };

            auto oracle = util::viewContainer(FAT.getAnnotations(f))
                         .map(llvm::dyn_caster<EnsuresAnnotation>{})
                         .filter()
                         .map([&](const EnsuresAnnotation* anno){
                             return cs.transform(anno->getTerm());
                          })
                         .toVector();

            testSuite->generateTest(testFile, fn, mit, oracle);
        }
    }
    
    testFile << "int main() {\n"
             << "    if (CUE_SUCCESS != CU_initialize_registry())\n"
             << "        return CU_get_error();\n";
    
    for (auto & f: M) {
        auto testSuite = tm->getTests(&f);
        if (testSuite != nullptr) {
            testSuite->activateTest(testFile);
        }
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
