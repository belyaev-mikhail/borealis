/* 
 * File:   TestSuite.cpp
 * Author: maxim
 * 
 * Created on 12 Февраль 2014 г., 13:50
 */

#include "TestGen/TestSuite.h"
#include "TestGen/Util/c_types.h"

#include "Util/macros.h"

namespace borealis {

TestSuite::TestSuite(const llvm::Function * f) : function(f) {
    generateResultVariableName();
}

TestSuite::TestSuite(const llvm::Function * f, const TestSet& tests) :
        function(f), tests(tests) {
    generateResultVariableName();
}

void TestSuite::addTestCase(const TestCase & testCase) {
    tests.insert(testCase);
}

void TestSuite::addTestSuite(const TestSuite& other) {
    // XXX Comparing llvm::Functions by name?!?!
    ASSERT(function->getName() == other.function->getName(),
            "Trying to test cases from test suite for another function.");
    for(const auto& testCase: other.tests) {
        tests.insert(testCase);
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

std::string TestSuite::getResultVariableName() const {
    return resultVariableName;
}

void TestSuite::generateResultVariableName() {
    resultVariableName = "result";
    while (true) {
        bool uniq = true;
        for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
            auto arg_ = const_cast<llvm::Argument *>(&(*arg));
            if (arg_->getName() == resultVariableName) {
                uniq = false;
                break;
            }
        }
        if (uniq) {
            break;
        }
        resultVariableName += util::toString(rand() % 10);
    }
}

} /* namespace borealis */
