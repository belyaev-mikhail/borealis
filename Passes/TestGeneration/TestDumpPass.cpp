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

#include "Passes/TestGeneration/TestDumpPass.h"

#include "Passes/Defect/DefectManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "Passes/Tracker/FunctionAnnotationTracker.h"

#include "poolalloc/src/DSA/stl_util.h"

#include "State/Transformer/ContractStupidifier.h"
#include "TestGen/CUnit/CUnitModule.h"
#include "TestGen/CUnit/CUnitMain.h"

#include "Util/filename_utils.h"
#include "Util/util.h"
#include "Util/json.hpp"

namespace borealis {

TestDumpPass::TestDumpPass() : ModulePass(ID) {}

void TestDumpPass::getAnalysisUsage(llvm::AnalysisUsage & AU) const {
    AU.setPreservesAll();

    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
    AUX<MetaInfoTracker>::addRequiredTransitive(AU);
    AUX<TestGenerationPass>::addRequiredTransitive(AU);
    AUX<TestManager>::addRequiredTransitive(AU);
    AUX<FunctionAnnotationTracker>::addRequiredTransitive(AU);
    AUX<prototypesLocation>::addRequiredTransitive(AU);
}

bool TestDumpPass::runOnModule(llvm::Module & M) {
    
    static config::StringConfigEntry testDirectoryEntry("testgen", "output-directory");
    static config::StringConfigEntry testPrefixEntry("testgen", "output-prefix");
    static config::StringConfigEntry testFormatEntry("testgen", "format");
    auto testDirectory = testDirectoryEntry.get("tests");

    auto testFormat = testFormatEntry.get("cunit");
    bool exists;
    
    llvm::sys::fs::create_directories(testDirectory, exists);
    
    tm = &GetAnalysis<TestManager>::doit(this);
    auto * stp = &GetAnalysis<SlotTrackerPass>::doit(this);
    mit = &GetAnalysis<MetaInfoTracker>::doit(this);
    
    FunctionAnnotationTracker& FAT = GetAnalysis<FunctionAnnotationTracker>::doit(this);

    auto * protoLoc = &GetAnalysis<prototypesLocation>::doit(this);
    prototypes = protoLoc->provide();

    auto* CUs = M.getNamedMetadata("llvm.dbg.cu");
    if ("json" == testFormat) {
        for (unsigned i = 0; i < CUs->getNumOperands(); i++) {
            llvm::SmallString<256> testDir(testDirectory);

            llvm::DICompileUnit cu(CUs->getOperand(i));

            auto cuName = llvm::sys::path::stem(cu.getFilename());

            llvm::sys::path::append(testDir, testPrefixEntry.get("test") +  "_" +
                                    cuName + ".json");

            testFileName = testDir.str();

            baseDirectory = cu.getDirectory();

            testFile.open(testFileName.str(), std::ios::out);
            auto testMap = tm->getTestsForCompileUnit(cu);
            testFile << util::jsonify(*testMap);
            testFile.close();
        }
    } else if ("cunit" == testFormat) {
        std::ofstream testsHeaderFile;
        testsHeaderFile.open(testDirectory + "/tests.h", std::ios::out);

        std::ofstream testsMainFile;
        testsMainFile.open(testDirectory + "/tests_main.c", std::ios::out);


        for (unsigned i = 0; i < CUs->getNumOperands(); i++) {
            llvm::SmallString<256> testDir(testDirectory);

            llvm::DICompileUnit cu(CUs->getOperand(i));

            auto cuName = llvm::sys::path::stem(cu.getFilename());

            llvm::sys::path::append(testDir, testPrefixEntry.get("test") +  "_" +
                                    cuName + ".c");

            testFileName = testDir.str();

            baseDirectory = cu.getDirectory();

            testFile.open(testFileName.str(), std::ios::out);
            auto testMap = tm->getTestsForCompileUnit(cu);
            testFile << util::CUnitModule(*testMap, *stp, *mit, FAT, *protoLoc, baseDirectory, cuName, testFileName);
            testFile.close();
        }
        testsMainFile << util::CUnitMain(M);
        testsHeaderFile << util::CUnitHeader(M);

        testsHeaderFile.close();
        testsMainFile.close();
    }
    

    return true;
}

TestDumpPass::~TestDumpPass() {}

void TestDumpPass::generateHeader(const std::vector<llvm::Function*> & funcs) {
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


char TestDumpPass::ID;
static RegisterPass<TestDumpPass>
X("test-dump", "Pass that dumps unit tests.");

} /* namespace borealis */
