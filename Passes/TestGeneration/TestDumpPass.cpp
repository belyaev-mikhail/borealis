/* 
 * File:   CUnitDumperPass.cpp
 * Author: maxim
 * 
 * Created on 18 Февраль 2014 г., 17:16
 */
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Driver/Arg.h>
#include <clang/Driver/ArgList.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Driver/Options.h>
#include <clang/Driver/Tool.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Frontend/Utils.h>
#include <llvm/Analysis/DebugInfo.h>
#include <llvm/Function.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/PathV2.h>

#include "Actions/TestGen/LocateFunctionsAction.h"
#include "Actions/TestGen/LocateIncludesAction.h"

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
#include "TestGen/SourceLocations.h"
#include "State/Transformer/ContractStupidifier.h"
#include "TestGen/CUnit/CUnitModule.h"
#include "TestGen/CUnit/CUnitMain.h"
#include "TestGen/CUnit/CUnitUserOracleStub.h"
#include "TestGen/CUnit/CUnitMakefile.h"
#include "TestGen/PrototypesInfo.h"
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

            makefile.setBaseDirectory(baseDirectory);
            makefile.addSource(cu.getFilename());
            makefile.addTest(testFileName);
            makefile.addOracle(oracleFilename(cuName));

            testFile.open(testFileName.str(), std::ios::out);
            auto testMap = tm->getTestsForCompileUnit(cu);
            testFile << util::CUnitModule(*testMap, *stp, *mit, FAT, *protoLoc, cuName, baseDirectory);
            testFile.close();

            auto funcs = util::viewContainer(*testMap)
                .map([](decltype(*testMap->begin()) pair) { return pair.first; });
            std::unordered_set<const llvm::Function*> functionOracles;

            LocationAnalyseResult::Ptr srcLocs = nullptr;
            LocationAnalyseResult::Ptr hdrLocs = nullptr;
            if ("preserve" == generateUserOraclesStubs()) {
                srcLocs = analyzeFileWithCLang(oraclePath(cuName), baseDirectory, *this);
                hdrLocs = analyzeFileWithCLang(oracleHeaderPath(cuName), baseDirectory, *this);
            }
            auto srcToInsert = getFunctionsToInsertOracles(srcLocs.get(), testMap.get());
            auto hdrToInsert = getFunctionsToInsertOracles(hdrLocs.get(), testMap.get());
            util::createOrUpdateOracleFile<util::CUnitUserOracleStubModule>(
                    oraclePath(cuName),
                    srcLocs,
                    srcToInsert,
                    *stp, *mit, *protoLoc,
                    cuName.str(),
                    baseDirectory);
            util::createOrUpdateOracleFile<util::CUnitUserOracleStubHeader>(
                    oracleHeaderPath(cuName),
                    hdrLocs,
                    hdrToInsert,
                    *stp, *mit, *protoLoc,
                    cuName.str(),
                    baseDirectory);
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



std::unordered_set<const llvm::Function*> TestDumpPass::getFunctionsToInsertOracles(
        LocationAnalyseResult* locations, TestManager::TestMap* testMap) {
    auto funcs = util::viewContainer(*testMap)
                 .map([](decltype(*testMap->begin()) pair) { return pair.first; });
    std::unordered_set<const llvm::Function*> functionOracles;
    if ("preserve" == generateUserOraclesStubs()) {
        if (nullptr != locations) {
            auto oldOraclesNames = util::view(locations->functions_begin(), locations->functions_end())
                .map([](decltype((*locations->functions_begin())) func) { return func.name; });

            auto toAdd = funcs.filter(
                [&](const llvm::Function* func) {
                    return !oldOraclesNames.any_of(
                        [&](const std::string& oldFuncName) {
                            return func->getName().str() + "Oracle" == oldFuncName;
                        });
                }
            );
            functionOracles.insert(toAdd.begin(), toAdd.end());
        } else {
            functionOracles.insert(funcs.begin(), funcs.end());
        }
    } else if ("override" == generateUserOraclesStubs()) {
        functionOracles.insert(funcs.begin(), funcs.end());
    }
    return std::move(functionOracles);
}

LocationAnalyseResult::Ptr TestDumpPass::analyzeFileWithCLang(const std::string& fileName,
        const std::string& baseDirectory,
        logging::ClassLevelLogging<TestDumpPass>& logging) {

    if (!llvm::sys::fs::exists(fileName))
            return nullptr;
    driver::CommandLine compilerArgs;
    compilerArgs = compilerArgs.push_back(fileName);

    if (includeInMakefile()) {
        if (absoluteInclude()) {
            compilerArgs = compilerArgs.push_back("-I");
            compilerArgs = compilerArgs.push_back(baseDirectory);
            compilerArgs = compilerArgs.push_back("-I");
            compilerArgs = compilerArgs.push_back(util::getAbsolutePath(baseDirectory, oracleDirectory()));
        } else {
            compilerArgs = compilerArgs.push_back("-I");
            compilerArgs = compilerArgs.push_back(util::getRelativePath(baseDirectory, "", baseDirectory));
            compilerArgs = compilerArgs.push_back("-I");
            compilerArgs = compilerArgs.push_back(util::getRelativePath(baseDirectory,
                    oracleDirectory(), baseDirectory));
        }
    }

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
    clang::CompilerInstance ci;
    ci.setDiagnostics(diags.getPtr());
    std::unique_ptr<clang::CompilerInvocation> CI { new clang::CompilerInvocation() };
    std::vector<const char*> ccArgs;
    auto cmdIt = std::find_if(
            compileCommands.begin(),
            compileCommands.end(),
            [](driver::command& cmd) {
                return (cmd.operation == driver::command::COMPILE);
            }
        );
    if (cmdIt == compileCommands.end()) {
        return nullptr;
    }
    auto& args = cmdIt->cl;
    ccArgs.reserve(args->getNumInputArgStrings());
    for (auto i = 0U, size = args->getNumInputArgStrings(); i < size; ++i) {
        auto* arg = args->getArgString(i);
        ccArgs.push_back(arg);
    }
    {
        auto info = infos();
        info << "cc " << borealis::util::head(ccArgs);
        for (const auto& arg: borealis::util::tail(ccArgs)) info << " " << arg;
        info << endl;
    }
    if (!clang::CompilerInvocation::CreateFromArgs(
            *CI,
            const_cast<const char**>(ccArgs.data()),
            const_cast<const char**>(ccArgs.data()) + ccArgs.size(),
             ci.getDiagnostics()))
        return nullptr;

    ci.setInvocation(CI.release());

    if (!ci.hasInvocation())
        return nullptr;
    IncludesLocations includes;
    LocateIncludesAction locateIncludes(&includes);
    ci.ExecuteAction(locateIncludes);
    FunctionsLocations locations;
    LocateFunctionsAction locateFunctions(&locations);
    ci.ExecuteAction(locateFunctions);

    if (ci.getDiagnostics().getClient()->getNumErrors() != 0)
        return nullptr;
    else
        return std::make_shared<LocationAnalyseResult>(
                includes[fileName], locations[fileName]
        );
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
    return testDir.str();
}

std::string TestDumpPass::oracleDirectory() {
    static config::StringConfigEntry userOraclesDirEntry("testgen", "user-oracles-directory");
    return userOraclesDirEntry.get("tests/oracles");
}

std::string TestDumpPass::oracleFilename(const std::string& moduleName) {
    static config::StringConfigEntry userOraclesPrefixEntry("testgen", "user-oracles-prefix");
    return userOraclesPrefixEntry.get("oracles") +  "_" +
            moduleName + ".c";
}

std::string TestDumpPass::oracleHeaderFilename(const std::string& moduleName) {
    static config::StringConfigEntry userOraclesPrefixEntry("testgen", "user-oracles-prefix");
    return userOraclesPrefixEntry.get("oracles") +  "_" +
            moduleName + ".h";
}

std::string TestDumpPass::oraclePath(const std::string& moduleName) {
    static config::StringConfigEntry userOraclesPrefixEntry("testgen", "user-oracles-prefix");
    llvm::SmallString<256> oraclesName(oracleDirectory());
    llvm::sys::path::append(oraclesName, oracleFilename(moduleName));
    return oraclesName.str().str();
}

std::string TestDumpPass::oracleHeaderPath(const std::string& moduleName) {
    static config::StringConfigEntry userOraclesPrefixEntry("testgen", "user-oracles-prefix");
    llvm::SmallString<256> oraclesHeaderName(oracleDirectory());
    llvm::sys::path::append(oraclesHeaderName, oracleHeaderFilename(moduleName));
    return oraclesHeaderName.str().str();
}

std::string TestDumpPass::generateUserOraclesStubs() {
    static config::StringConfigEntry generateUserOraclesEntry("testgen", "generate-user-oracles-stub");
    return generateUserOraclesEntry.get("false");

}


TestDumpPass::~TestDumpPass() {}

char TestDumpPass::ID;
static RegisterPass<TestDumpPass>
X("test-dump", "Pass that dumps unit tests.");

} /* namespace borealis */
