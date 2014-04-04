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
#include "TestGen/PrototypesInfo.h"
#include "TestGen/TestCase.h"
#include "Util/json_traits.hpp"

namespace borealis {

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
    
    void prototypeFunction(std::ostream & outStream, const MetaInfoTracker * mit,
        const PrototypesInfo* prototypes) const;
    void generateTest(std::ostream & outStream, FactoryNest fn,
        MetaInfoTracker * mit, const std::vector<Term::Ptr>& oracle);
    void activateTest(std::ostream & outStream) const;
    
    std::string getTestName() const;
    llvm::StringRef getFunctionName() const;
    std::string getSuiteName() const;
    std::string getResultVariableName() const;

    const_iterator begin() const { return tests.begin(); }
    const_iterator end() const { return tests.end(); }

private:
    void generateResultVariableName();
    
    const llvm::Function * function;
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

