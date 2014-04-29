/* 
 * File:   TestCase.cpp
 * Author: maxim
 * 
 * Created on 12 Февраль 2014 г., 13:51
 */

#include <llvm/Analysis/DebugInfo.h>

#include "TestGen/TestCase.h"
#include "TestGen/TestSuite.h"
#include "TestGen/Util/c_types.h"
#include "Util/util.h"

namespace borealis {

TestCase::TestCase(const TestCase::TermMap & testCase) :
        testCase(testCase){}

void TestCase::addArgument(const Term::Ptr arg, const Term::Ptr value) {
    testCase.emplace(arg, value);
}

std::string TestCase::getTestName(const llvm::Function * function, int id) const {
    return getTestName(function->getName(), id);
}

std::string TestCase::getTestName(llvm::StringRef functionName, int id) const {
    return "test" + util::capitalize(functionName.str()) + "_" + util::toString(id);
}

const Term::Ptr TestCase::getValue(const Term::Ptr arg) const {
    return testCase.at(arg);
}

} /* namespace borealis */
