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
#include "TestGen/TestCase.h"


namespace borealis {

class TestSuite {
public:
    typedef std::shared_ptr<TestSuite> Ptr;

    TestSuite() = delete;
    TestSuite(const TestSuite & orig) = default;
    TestSuite(TestSuite && orig) = default;
    TestSuite(const llvm::Function * f);
    TestSuite(const llvm::Function * f, const std::unordered_set<TestCase> & tests);
    
    void addTestCase(const TestCase & testCase);
    void addTestSuite(const TestSuite& other);
    
    void prototypeFunction(std::ostream & outStream, MetaInfoTracker * mit) const;
    void generateTest(std::ostream & outStream, FactoryNest fn,
        MetaInfoTracker * mit, const std::vector<Term::Ptr>& oracle);
    void activateTest(std::ostream & outStream) const;
    
    std::string getTestName() const;
    llvm::StringRef getFunctionName() const;
    std::string getSuiteName() const;
    std::string getResultVariableName() const;
private:
    void generateResultVariableName();
    
    const llvm::Function * function;
    std::unordered_set<TestCase> tests;
    std::string resultVariableName;
};

} /* namespace borealis */

#endif	/* TESTSUITE_H */

