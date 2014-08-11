/* 
 * File:   TestStatisticsDumpPass.h
 * Author: maxim
 *
 * Created on 11 Август 2014 г., 15:11
 */

#ifndef TESTSTATISTICSDUMPPASS_H
#define	TESTSTATISTICSDUMPPASS_H

#include <llvm/Pass.h>

#include "Logging/logger.hpp"
#include "Passes/TestGeneration/FunctionInfoPass.h"
#include "TestGen/TestStatistics.h"

#include "Util/passes.hpp"

namespace borealis {

class TestStatisticsDumpPass :
        public llvm::ModulePass,
        public borealis::logging::ClassLevelLogging<TestStatisticsDumpPass> {
public:
    
    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("test-statistics-dumper")
#include "Util/unmacros.h"
    
    TestStatisticsDumpPass();
    virtual ~TestStatisticsDumpPass();
    
    virtual bool runOnModule(llvm::Module & M) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage & AU) const override;
private:
    
    TestStatistics* stat;

};

} /* namespace borealis */

#endif	/* TESTSTATISTICSDUMPPASS_H */

