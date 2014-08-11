/* 
 * File:   CUnitDumperPass.h
 * Author: maxim
 *
 * Created on 18 Февраль 2014 г., 17:16
 */

#ifndef CUNITDUMPERPASS_H
#define	CUNITDUMPERPASS_H

#include <fstream>
#include <llvm/Metadata.h>
#include <llvm/Pass.h>

#include "Driver/AnnotatedModule.h"
#include "Factory/Nest.h"
#include "Logging/logger.hpp"
#include "Passes/TestGeneration/FunctionInfoPass.h"
#include "Passes/TestGeneration/TestManager.h"
#include "Passes/Util/DataProvider.hpp"
#include "TestGen/FunctionsInfoData.h"
#include "TestGen/TestStatistics.h"
#include "TestGen/SourceLocations.h"

#include "Util/passes.hpp"

namespace borealis {

class TestDumpPass :
        public llvm::ModulePass,
        public borealis::logging::ClassLevelLogging<TestDumpPass> {
public:

    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("cunit-dumper")
#include "Util/unmacros.h"

    TestDumpPass();
    
    typedef DataProvider<FunctionsInfoData> FInfoData;
    
    virtual bool runOnModule(llvm::Module & M) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage & AU) const override;
    virtual ~TestDumpPass();
private:
    TestManager * tm;
    FunctionInfoPass * fip;
    TestStatistics * ts;
    std::ofstream testFile;
    llvm::StringRef testFileName;
    llvm::StringRef baseDirectory;
    FunctionsInfoData fInfoData;

public:
    std::unordered_set<const llvm::Function*> getFunctionsToInsertOracles(
            LocationAnalyseResult* locations, TestManager::TestMap* testMap);
    LocationAnalyseResult::Ptr analyzeFileWithCLang(const std::string& fileName,
            const std::string& baseDirectory,
            logging::ClassLevelLogging<TestDumpPass>& logging);
    static std::shared_ptr<std::unordered_set<llvm::Function*>> getFunctionsToGenerateOracleStubs(
            std::unordered_set<llvm::Function*> funcs);
    static std::string getResultNameForFunction(
            const llvm::Function* function, const std::string& dflt = "result");
    static bool insertUserOraclesCall();
    static bool absoluteInclude();
    static bool includeInMakefile();
    static std::string filePathForModule(const std::string& moduleName);
    static std::string oracleDirectory();
    static std::string oracleFilename(const std::string& moduleName);
    static std::string oracleHeaderFilename(const std::string& moduleName);
    static std::string oraclePath(const std::string& moduleName);
    static std::string oracleHeaderPath(const std::string& moduleName);
    static std::string oracleFilePath(const std::string& moduleName);
    static std::string generateUserOraclesStubs();
    static std::string getCompileUnitFilename(const std::string& cuFilename, const FunctionsInfoData& fInfoData);
    std::string getCompileUnitFilename(const std::string& cuFilename);
};

} /* namespace borealis */

#endif	/* CUNITDUMPERPASS_H */

