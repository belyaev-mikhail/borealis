/* 
 * File:   TestUnit.cpp
 * Author: maxim
 * 
 * Created on 14 Февраль 2014 г., 12:39
 */

#include "TestGen/TestUnit.h"

namespace borealis {

TestUnit::TestUnit(FactoryNest fn) : fn(fn) {}

TestSuite & TestUnit::addTestSuite(const TestSuite & suite) {
    functions.push_back(suite);
    return functions.back();
}

TestSuite & TestUnit::addTestSuite(llvm::Function * f) {
    TestSuite suite(f, fn);
    return addTestSuite(suite);
}

TestSuite & TestUnit::addTestSuite(llvm::Function * f, const std::vector<TestCase> & tests) {
    TestSuite suite(f, tests, fn);
    return addTestSuite(suite);
}

void TestUnit::generateTest(std::ostream & outStream) {
    for (const auto & f: functions) {
        f.generateTest(outStream);
    }
}

TestUnit::~TestUnit() {
}

} /* namespace borealis */
