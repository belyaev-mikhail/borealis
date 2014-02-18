/* 
 * File:   TestCase.cpp
 * Author: maxim
 * 
 * Created on 12 Февраль 2014 г., 13:51
 */

#include "TestGen/TestCase.h"

namespace borealis {

TestCase::TestCase(const TestCase::TermMap & testCase) :
        testCase(testCase){}

void TestCase::addArgument(const Term::Ptr arg, const Term::Ptr value) {
    testCase.emplace(arg, value);
}

const Term::Ptr TestCase::getValue(const Term::Ptr arg) const {
    return testCase.at(arg);
}

TestCase::~TestCase() {
}

} /* namespace borealis */
