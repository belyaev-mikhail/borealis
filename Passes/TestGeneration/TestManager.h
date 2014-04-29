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
#include "Util/json.hpp"

namespace borealis {

class TestManager :
        public llvm::ImmutablePass,
        public borealis::logging::ClassLevelLogging<TestManager> {
public:
    
    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("TestManager")
#include "Util/unmacros.h"
    typedef std::unordered_map<const llvm::Function*,
            TestSuite::Ptr> TestMap;
    typedef std::shared_ptr<TestMap> TestMapPtr;
    TestManager();
    virtual ~TestManager() {};
    
    virtual void getAnalysisUsage(llvm::AnalysisUsage & AU) const override;
    
    void put(const llvm::Function* F, TestSuite::Ptr tests);
    void update(const llvm::Function* F, TestSuite::Ptr tests);
    TestSuite::Ptr getTests(const llvm::Function* F) const;
    TestMapPtr getTestsForCompileUnit(llvm::DICompileUnit& unit) const;
private:
    TestMap functionTests;

};

template<>
struct util::json_traits<TestManager::TestMap> {
    typedef std::unique_ptr<TestManager::TestMap> optional_ptr_t;

    static Json::Value toJson(const TestManager::TestMap& val) {
        Json::Value ret;
        for(const auto& m : val) ret[m.first->getName()] = util::toJson(*m.second);
        return ret;
    }

};

} /* namespace borealis */

#endif	/* TESTMANAGER_H */

