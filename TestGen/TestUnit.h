/* 
 * File:   TestUnit.h
 * Author: maxim
 *
 * Created on 14 Февраль 2014 г., 12:38
 */

#ifndef TESTUNIT_H
#define	TESTUNIT_H

#include <llvm/Function.h>

#include "Factory/Nest.h"
#include "TestGen/TestSuite.h"


namespace borealis {

class TestUnit {
public:
    explicit TestUnit(FactoryNest fn);
    TestUnit(const TestUnit& orig) = default;
    TestSuite & addTestSuite(const TestSuite & suite);
    TestSuite & addTestSuite(llvm::Function * f);
    TestSuite & addTestSuite(llvm::Function * f, const std::vector<TestCase> & tests);
    void generateTest(std::ostream & outStream);
    virtual ~TestUnit();
private:
    std::vector<TestSuite> functions;
    FactoryNest fn;
};

} /* namespace borealis */

#endif	/* TESTUNIT_H */

