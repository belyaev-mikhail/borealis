/* 
 * File:   TestManager.h
 * Author: maxim
 *
 * Created on 3 Март 2014 г., 16:51
 */

#ifndef TESTMANAGER_H
#define	TESTMANAGER_H

#include <llvm/Pass.h>

#include "Logging/logger.hpp"
#include "TestGen/TestSuite.h"

namespace borealis {

class TestManager :
        public llvm::ImmutablePass,
        public borealis::logging::ClassLevelLogging<TestManager> {
public:
    
    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("tm")
#include "Util/unmacros.h"
    
    TestManager();
    virtual ~TestManager() {};
    
    virtual void getAnalysisUsage(llvm::AnalysisUsage & AU) const override;
    
    void put(const llvm::Function * F, TestSuite::Ptr tests);
    void update(const llvm::Function * F, TestSuite::Ptr tests);
    TestSuite::Ptr getTests(const llvm::Function * F) const;

private:
    std::unordered_map<const llvm::Function *, TestSuite::Ptr> functionTests;

};

} /* namespace borealis */

#endif	/* TESTMANAGER_H */

