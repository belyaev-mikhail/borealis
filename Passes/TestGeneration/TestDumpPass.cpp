/* 
 * File:   CUnitDumperPass.cpp
 * Author: maxim
 * 
 * Created on 18 Февраль 2014 г., 17:16
 */
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Driver/ArgList.h>
#include <clang/Driver/Compilation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/DiagnosticOptions.h>
#include <llvm/Analysis/DebugInfo.h>
#include <llvm/Function.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/PathV2.h>

#include "Driver/interviewer.h"
#include "Passes/TestGeneration/TestDumpPass.h"

#include "Passes/Defect/DefectManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "Passes/Tracker/FunctionAnnotationTracker.h"

#include "poolalloc/src/DSA/stl_util.h"

#include "Codegen/DiagnosticLogger.h"
#include "Driver/AnnotatedModule.h"
#include "Driver/clang_pipeline.h"
#include "State/Transformer/ContractStupidifier.h"
#include "TestGen/CUnit/CUnitModule.h"
#include "TestGen/CUnit/CUnitMain.h"
#include "TestGen/CUnit/CUnitUserOracleStub.h"
#include "TestGen/CUnit/CUnitMakefile.h"
#include "TestGen/util.h"



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
    AUX<PrototypesLocation>::addRequiredTransitive(AU);
}

bool TestDumpPass::runOnModule(llvm::Module & M) {
    
    static config::StringConfigEntry testDirectoryEntry("testgen", "output-directory");
    static config::StringConfigEntry testPrefixEntry("testgen", "output-prefix");
    static config::StringConfigEntry testFormatEntry("testgen", "format");
    static config::StringConfigEntry generateUserOraclesEntry("testgen", "generate-user-oracles-stub");
    static config::StringConfigEntry userOraclesDirEntry("testgen", "user-oracles-directory");
    static config::StringConfigEntry userOraclesPrefixEntry("testgen", "user-oracles-prefix");



    auto testDirectory = testDirectoryEntry.get("tests");

    auto testFormat = testFormatEntry.get("cunit");
    bool exists;
    
    auto generateOracleStubs = generateUserOraclesEntry.get("false");
    auto oraclesDirectory = userOraclesDirEntry.get("tests/oracles");

    llvm::sys::fs::create_directories(testDirectory, exists);
    llvm::sys::fs::create_directories(oraclesDirectory, exists);
    tm = &GetAnalysis<TestManager>::doit(this);
    auto * stp = &GetAnalysis<SlotTrackerPass>::doit(this);
    mit = &GetAnalysis<MetaInfoTracker>::doit(this);
    
    FunctionAnnotationTracker& FAT = GetAnalysis<FunctionAnnotationTracker>::doit(this);

    auto * protoLoc = &GetAnalysis<PrototypesLocation>::doit(this);
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
        std::string testsHeaderFilename = testDirectory + "/tests.h";
        std::ofstream testsHeaderFile;
        testsHeaderFile.open(testsHeaderFilename, std::ios::out);

        std::string testsMainFilename = testDirectory + "/tests_main.c";
        std::ofstream testsMainFile;
        testsMainFile.open(testsMainFilename, std::ios::out);

        std::string testsMakefileName = testDirectory + "/Makefile";
        std::ofstream testsMakefile;
        testsMakefile.open(testsMakefileName, std::ios::out);

        auto makefile = util::CUnitMakefile(M);
        
        makefile.setName(testsMakefileName);
        
        for (unsigned i = 0; i < CUs->getNumOperands(); i++) {
            llvm::SmallString<256> testDir(testDirectory);

            llvm::DICompileUnit cu(CUs->getOperand(i));
            auto cuName = llvm::sys::path::stem(cu.getFilename());

            llvm::sys::path::append(testDir, testPrefixEntry.get("test") +  "_" +
                                    cuName + ".c");

            testFileName = testDir.str();
            
            baseDirectory = cu.getDirectory();
            
            llvm::SmallString<256> oraclesFileName(oraclesDirectory);
            llvm::sys::path::append(oraclesFileName, userOraclesPrefixEntry.get("oracles") +  "_" +
                                                cuName + ".c");
            llvm::SmallString<256> oraclesHeaderName(oraclesDirectory);
            llvm::sys::path::append(oraclesHeaderName, userOraclesPrefixEntry.get("oracles") +  "_" +
                                                cuName + ".h");

            makefile.setBaseDirectory(baseDirectory);
            makefile.addSource(cu.getFilename());
            makefile.addTest(testFileName);
            makefile.addOracle(oraclesFileName.str());

            testFile.open(testFileName.str(), std::ios::out);
            auto testMap = tm->getTestsForCompileUnit(cu);
            testFile << util::CUnitModule(*testMap, *stp, *mit, FAT, *protoLoc, cuName, baseDirectory);
            testFile.close();

            auto funcs = util::viewContainer(*testMap)
                .map([](decltype(*testMap->begin()) pair) { return pair.first; });
            std::unordered_set<const llvm::Function*> functionOracles;
            bool regenerateFlag = false;
            if ("preserve" == generateOracleStubs) {
                auto oracleAM = compileFileWithCLang(oraclesFileName.str(), *this);
                if (nullptr != oracleAM) {
                    auto oracleModule = oracleAM->module;
                    auto oldOraclesView = util::view(oracleModule->begin(), oracleModule->end())
                        .map([](decltype((*oracleModule->begin())) func) { return &func; });
                    auto toAdd = funcs.filter(
                        [&](const llvm::Function* func) {
                            return !oldOraclesView.any_of(
                                [&](const llvm::Function* oldFunc) {
                                    return func->getName().str() + "Oracle" == oldFunc->getName().str();
                                });
                        }
                    );
                    functionOracles.insert(toAdd.begin(), toAdd.end());
                } else {
                    functionOracles.insert(funcs.begin(), funcs.end());
                    regenerateFlag = true;
                }
            } else if ("override" == generateOracleStubs) {
                functionOracles.insert(funcs.begin(), funcs.end());
            }
            if ("preserve" == generateOracleStubs || "override" == generateOracleStubs) {
                std::ofstream oracleFile;
                oracleFile.open(oraclesFileName.str(),
                        "override" == generateOracleStubs || regenerateFlag ? std::ios::out : std::ios::app);
                oracleFile << util::CUnitUserOracleStubModule(functionOracles, *stp, *mit, *protoLoc,
                        cuName.str(), baseDirectory);
                std::ofstream oracleHeader;
                oracleHeader.open(oraclesHeaderName.str(),
                        "override" == generateOracleStubs || regenerateFlag ? std::ios::out : std::ios::app);
                oracleHeader << util::CUnitUserOracleStubHeader(functionOracles, *stp, *mit,
                        *protoLoc, cuName.str(), baseDirectory);
                oracleFile.close();
                oracleHeader.close();

            }
        }
        
        makefile.addTest(testsMainFilename);
        
        testsMainFile << util::CUnitMain(M);
        testsHeaderFile << util::CUnitHeader(M);
        testsMakefile << makefile;

        testsHeaderFile.close();
        testsMainFile.close();
        testsMakefile.close();
    }
    

    return true;
}

driver::AnnotatedModule::Ptr TestDumpPass::compileFileWithCLang(const std::string& fileName,
        logging::ClassLevelLogging<TestDumpPass>& logging) {
    if (!llvm::sys::fs::exists(fileName))
        return nullptr;
    driver::CommandLine compilerArgs;
    compilerArgs = compilerArgs.push_back(fileName);

    infos() << "Invoking clang with: " << compilerArgs << endl;
    clang::DiagnosticOptions DiagOpts;
    DiagOpts.ShowCarets = false;

    auto diagBuffer = borealis::util::uniq(new DiagnosticLogger(logging));
    auto diags = clang::CompilerInstance::createDiagnostics(DiagOpts,
            compilerArgs.argc(), compilerArgs.argv(),
            diagBuffer.get(), /* ownClient = */false, /* cloneClient = */false);
    auto nativeClangCfg = borealis::config::StringConfigEntry("run", "clangExec");

    driver::interviewer nativeClang("clang", compilerArgs.data(), diags, nativeClangCfg);
    nativeClang.assignLogger(logging);
    auto compileCommands = nativeClang.getCompileCommands();
    if (compileCommands.empty()) return nullptr;
    driver::clang_pipeline clang("clang", diags);
    clang.assignLogger(logging);
    clang.invoke(compileCommands);
    auto annotatedModule = clang.result();
    return annotatedModule;
};

std::string TestDumpPass::getResultNameForFunction(const llvm::Function* function,
        const std::string& dflt) {
    auto resultVariableName = dflt;
    while (true) {
        bool uniq = true;
        for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
            auto arg_ = const_cast<llvm::Argument*>(&(*arg));
            if (arg_->getName() == resultVariableName) {
                uniq = false;
                break;
            }
        }
        if (uniq) {
            break;
        }
        resultVariableName += util::toString(rand() % 10);
    }
    return std::move(resultVariableName);
}

bool TestDumpPass::insertUserOraclesCall() {
    config::StringConfigEntry generateUserOraclesEntry("testgen", "generate-user-oracles-stub");
    auto generateOracleStubs = generateUserOraclesEntry.get("false");
    return "false" != generateOracleStubs ? true : false;
}

bool TestDumpPass::absoluteInclude() {
    config::BoolConfigEntry absoluteIncludeEntry("testgen", "absolute-include");
    return absoluteIncludeEntry.get(false);
}

bool TestDumpPass::includeInMakefile() {
    config::BoolConfigEntry includeInMakefileEntry("testgen", "include-in-makefile");
    return includeInMakefileEntry.get(true);
}

std::string TestDumpPass::filePathForModule(const std::string& moduleName) {
    config::StringConfigEntry testDirectoryEntry("testgen", "output-directory");
    config::StringConfigEntry testPrefixEntry("testgen", "output-prefix");
    auto testDirectory = testDirectoryEntry.get("tests");
    llvm::SmallString<256> testDir(testDirectory);
    llvm::sys::path::append(testDir, testPrefixEntry.get("test") +  "_" +
            moduleName + ".c");
    return std::move(testDir.str());
}

std::string TestDumpPass::oracleDirectory() {
    static config::StringConfigEntry userOraclesDirEntry("testgen", "user-oracles-directory");
    return std::move(userOraclesDirEntry.get("tests/oracles"));
}

std::string TestDumpPass::oracleHeaderFilename(const std::string& moduleName) {
    static config::StringConfigEntry userOraclesPrefixEntry("testgen", "user-oracles-prefix");
    return std::move(userOraclesPrefixEntry.get("oracles") +  "_" +
            moduleName + ".h");
}

std::string TestDumpPass::oracleHeaderPath(const std::string& moduleName) {
    static config::StringConfigEntry userOraclesPrefixEntry("testgen", "user-oracles-prefix");
    llvm::SmallString<256> oraclesHeaderName(oracleDirectory());
    llvm::sys::path::append(oraclesHeaderName, oracleHeaderFilename(moduleName));
    return std::move(oraclesHeaderName.str().str());
}



TestDumpPass::~TestDumpPass() {}

char TestDumpPass::ID;
static RegisterPass<TestDumpPass>
X("test-dump", "Pass that dumps unit tests.");

} /* namespace borealis */
