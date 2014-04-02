/* 
 * File:   CUnitDumperPass.cpp
 * Author: maxim
 * 
 * Created on 18 Февраль 2014 г., 17:16
 */

#include <llvm/Analysis/DebugInfo.h>
#include <llvm/Function.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/PathV2.h>

#include "Passes/TestGeneration/CUnitDumperPass.h"

#include "Passes/Defect/DefectManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "Passes/Tracker/FunctionAnnotationTracker.h"

#include "poolalloc/src/DSA/stl_util.h"

#include "State/Transformer/ContractStupidifier.h"
#include "Util/filename_utils.h"
#include "Util/util.h"

namespace borealis {

CUnitDumperPass::CUnitDumperPass() : ModulePass(ID) {}

void CUnitDumperPass::getAnalysisUsage(llvm::AnalysisUsage & AU) const {
    AU.setPreservesAll();

    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
    AUX<MetaInfoTracker>::addRequiredTransitive(AU);
    AUX<TestGenerationPass>::addRequiredTransitive(AU);
    AUX<TestManager>::addRequiredTransitive(AU);
    AUX<FunctionAnnotationTracker>::addRequiredTransitive(AU);
    AUX<prototypesLocation>::addRequiredTransitive(AU);
}

bool CUnitDumperPass::runOnModule(llvm::Module & M) {
    
    static config::StringConfigEntry testDirectoryEntry("cunit", "output-directory");
    static config::StringConfigEntry testPrefixEntry("cunit", "output-prefix");
    
    auto testDirectory = testDirectoryEntry.get("tests");
    
    bool exists;
    
    llvm::sys::fs::create_directories(testDirectory, exists);
    
    tm = &GetAnalysis<TestManager>::doit(this);
    auto * stp = &GetAnalysis<SlotTrackerPass>::doit(this);
    mit = &GetAnalysis<MetaInfoTracker>::doit(this);
    
    FunctionAnnotationTracker& FAT = GetAnalysis<FunctionAnnotationTracker>::doit(this);

    auto * protoLoc = &GetAnalysis<prototypesLocation>::doit(this);
    prototypes = protoLoc->provide();

    auto* CUs = M.getNamedMetadata("llvm.dbg.cu");
    
    std::ofstream testsHeaderFile;
    testsHeaderFile.open(testDirectory + "/tests.h", std::ios::out);
    
    std::ofstream testsMainFile;
    testsMainFile.open(testDirectory + "/tests_main.c", std::ios::out);
    
    testsMainFile << "#include <CUnit/Basic.h>\n\n";
    testsMainFile << "#include \"tests.h\"\n\n";
    
    testsMainFile << "int main() {\n";
    testsMainFile << "    CU_basic_set_mode(CU_BRM_VERBOSE);\n";
    
    for (unsigned i = 0; i < CUs->getNumOperands(); i++) {
    
        llvm::SmallString<256> testDir(testDirectory);
        
        llvm::DICompileUnit cu(CUs->getOperand(i));

        auto cuName = llvm::sys::path::stem(cu.getFilename());
        
        llvm::sys::path::append(testDir, testPrefixEntry.get("test") +  "_" +
                                cuName + ".c");

        testFileName = testDir.str();

        baseDirectory = cu.getDirectory();

        testFile.open(testFileName.str(), std::ios::out);
        
        auto SPs = cu.getSubprograms();
        
        std::vector<llvm::Function*> cuFunctions;
        
        for (unsigned i = 0; i < SPs.getNumElements(); i++) {
            llvm::DISubprogram sp(SPs.getElement(i));
            cuFunctions.push_back(sp.getFunction());
        }
        
        generateHeader(cuFunctions);

        for (auto & f: cuFunctions) {
            auto * st = stp->getSlotTracker(*f);
            auto fn = FactoryNest(st);

            auto testSuite = tm->getTests(f);

            if (testSuite != nullptr) {
                ContractStupidifier cs{
                    util::view(f->arg_begin(), f->arg_end())
                         .map([&](llvm::Argument& a){
                              return fn.Term->getArgumentTerm(&a);
                          })
                         .toVector(),
                    fn.Term->getValueTerm(fn.Type->getUnknownType(), testSuite->getResultVariableName()),
                    fn
                };

                auto oracle = util::viewContainer(FAT.getAnnotations(*f))
                             .map(llvm::dyn_caster<EnsuresAnnotation>{})
                             .filter()
                             .map([&](const EnsuresAnnotation* anno){
                                 return cs.transform(anno->getTerm());
                              })
                             .toVector();

                testSuite->generateTest(testFile, fn, mit, oracle);
            }
        }
        
        testFile << "int run" << util::capitalize(cuName) << "Test(void) {\n";
	testFile << "    if (CUE_SUCCESS != CU_initialize_registry())\n"
		 << "        return CU_get_error();\n";
        
        for (auto & f: cuFunctions) {
            auto testSuite = tm->getTests(f);
            testSuite->activateTest(testFile);
        }
        
        testFile << "    CU_basic_run_tests();\n";
        testFile << "    CU_cleanup_registry();\n";
        testFile << "    return 0;\n";
        testFile << "}\n";
        
        testsHeaderFile << "int run" << util::capitalize(cuName) << "Test(void);\n";
        
        testsMainFile << "    run" << util::capitalize(cuName) << "Test();\n";
        
        testFile.close();
    }
    
    testsHeaderFile.close();
    
    testsMainFile << "}\n";
    
    testsMainFile.close();
    
    return true;
}

CUnitDumperPass::~CUnitDumperPass() {}

void CUnitDumperPass::generateHeader(const std::vector<llvm::Function*> & funcs) {
    testFile << "#include <CUnit/Basic.h>\n\n";
    
    std::unordered_set<std::string> userIncludes;
    for (const auto& f : funcs) {
        auto loc = prototypes.locations.find(f->getName());
        if (loc != prototypes.locations.end()) {
            userIncludes.insert(loc->second);
        }
    }
    std::vector<std::string> includes(userIncludes.begin(), userIncludes.end());
    sort(includes.begin(), includes.end());
    for (const auto& i: includes) {
        testFile << "#include \"" << util::getRelativePath(baseDirectory, llvm::StringRef(i), llvm::StringRef(testFileName.str())) << "\"\n";
    }
    testFile << "\n";
    
    for (const auto& f: funcs) {
        auto testSuite = tm->getTests(f);
        if (testSuite != nullptr) {
            testSuite->prototypeFunction(testFile, mit, &prototypes);
        }
    }
    testFile << "\n";
}


char CUnitDumperPass::ID;
static RegisterPass<CUnitDumperPass>
X("cunit-dumper", "Pass that dumps unit tests into CUnit tests.");

} /* namespace borealis */
