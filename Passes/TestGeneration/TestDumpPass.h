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

#include "Factory/Nest.h"
#include "Logging/logger.hpp"
#include "Passes/TestGeneration/TestManager.h"
#include "Passes/Tracker/MetaInfoTracker.h"
#include "Passes/Util/DataProvider.hpp"
#include "TestGen/PrototypesInfo.h"
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
    
    typedef DataProvider<PrototypesInfo> prototypesLocation;
    
    virtual bool runOnModule(llvm::Module & M) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage & AU) const override;
    virtual ~TestDumpPass();
    
private:
    TestManager * tm;
    MetaInfoTracker * mit;
    std::ofstream testFile;
    llvm::StringRef testFileName;
    llvm::StringRef baseDirectory;
    PrototypesInfo prototypes;
};

} /* namespace borealis */

#endif	/* CUNITDUMPERPASS_H */

