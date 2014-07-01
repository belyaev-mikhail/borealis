/* 
 * File:   TestSuite.h
 * Author: maxim
 *
 * Created on 12 Февраль 2014 г., 13:50
 */

#ifndef TESTSUITE_H
#define	TESTSUITE_H

#include <llvm/Function.h>

#include "Factory/Nest.h"
#include "Passes/Tracker/MetaInfoTracker.h"
#include "TestGen/FunctionsInfoData.h"
#include "TestGen/FunctionInfo.h"
#include "TestGen/TestCase.h"
#include "Util/json_traits.hpp"

namespace borealis {

class FunctionInfoPass;
    
class TestSuite {
private:
    typedef std::unordered_set<TestCase> TestSet;
    typedef TestSet::const_iterator const_iterator;
public:
    typedef std::shared_ptr<TestSuite> Ptr;


    TestSuite() = delete;
    TestSuite(const TestSuite & orig) = default;
    TestSuite(TestSuite && orig) = default;
    TestSuite(const llvm::Function * f);
    TestSuite(const llvm::Function * f, const TestSet& tests);
    
    void addTestCase(const TestCase & testCase);
    void addTestSuite(const TestSuite& other);
    std::string getTestName() const;
    llvm::StringRef getFunctionName() const;
    const llvm::Function* getFunction() const { return fi->getFunction(); }
    const FunctionInfo* getFunctionInfo() const { return fi; }
    std::string getSuiteName() const;
    std::string getResultVariableName() const;
    
    void setFunctionInfo(FunctionInfoPass& fip);

    const_iterator begin() const { return tests.begin(); }
    const_iterator end() const { return tests.end(); }
    bool empty() const { return tests.empty(); }
    const std::string& getResultVariableName() { return resultVariableName; }
private:
    void generateResultVariableName();
    
    const llvm::Function* function;
    const FunctionInfo* fi;
    TestSet tests;
    std::string resultVariableName;
};

namespace util {
////////////////////////////////////////////////////////////////////////////////
// Json
////////////////////////////////////////////////////////////////////////////////
template<>
struct json_traits<TestSuite> {
    typedef std::unique_ptr<TestSuite> optional_ptr_t;

    static Json::Value toJson(const TestSuite& val) {
        Json::Value dict;
        dict["function"] = util::toJson(std::string(val.getFunctionName()));
        Json::Value cases;
        for (auto c : val) {
            cases.append(util::toJson(c));
        }
        dict["cases"] = cases;
        return dict;
    }
};
} // namespace util

} /* namespace borealis */

#endif	/* TESTSUITE_H */

