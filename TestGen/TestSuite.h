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
#include "TestGen/TestCase.h"


namespace borealis {

class TestSuite {
public:
    TestSuite() = delete;
    TestSuite(const TestSuite & orig) = default;
    TestSuite(TestSuite && orig) = default;
    TestSuite(llvm::Function * f);
    TestSuite(llvm::Function * f, const std::vector<TestCase> & tests);
    void addTestCase(const TestCase & testCase);
    void generateTest(std::ostream & outStream, FactoryNest fn) const;
    virtual ~TestSuite();
private:
    llvm::Function * function;
    std::vector<TestCase> tests;
};

} /* namespace borealis */

#endif	/* TESTSUITE_H */

