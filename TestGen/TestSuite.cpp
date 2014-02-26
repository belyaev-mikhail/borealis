/* 
 * File:   TestSuite.cpp
 * Author: maxim
 * 
 * Created on 12 Февраль 2014 г., 13:50
 */

#include "TestGen/TestSuite.h"

#include "Util/macros.h"

namespace borealis {

TestSuite::TestSuite(const llvm::Function * f) : function(f) {}

TestSuite::TestSuite(const llvm::Function * f, const std::vector<TestCase> & tests) :
        function(f), tests(tests) {}

void TestSuite::addTestCase(const TestCase & testCase) {
    tests.push_back(testCase);
}

void TestSuite::addTestSuite(const TestSuite& other) {
    // XXX Comparing llvm::Functions by name?!?!
    ASSERT(function->getName() == other.function->getName(),
            "Trying to test cases from test suite for another function.");
    for(const auto& testCase: other.tests) {
        tests.push_back(testCase);
    }
}

void TestSuite::generateTest(std::ostream & outStream, FactoryNest fn) {
    if (tests.empty()) {
        return;
    }
    for (size_t i = 0; i < tests.size(); i++) {
        tests[i].generateTest(outStream, function, fn, i);
    }
}

void TestSuite::activateTest(std::ostream & outStream) const {
    if (tests.empty()) {
        return;
    }
    outStream << "    CU_pSuite " << getSuiteName() << " = CU_add_suite(\"Suite for " << getFunctionName() << "\", NULL, NULL);\n"
            << "    if (" << getSuiteName() << " == NULL) {\n"
            << "        CU_cleanup_registry();\n"
            << "        return CU_get_error();\n"
            << "    }\n";
    for (const auto & t: tests) {
        t.activateTest(outStream, function);
    }
}


std::string TestSuite::getTestName() const {
    return "test" + function->getName().upper();
}

llvm::StringRef TestSuite::getFunctionName() const {
    return function->getName();
}

std::string TestSuite::getSuiteName() const {
    return "pSuite" + function->getName().upper();
}

} /* namespace borealis */
