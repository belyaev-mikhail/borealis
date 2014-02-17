/* 
 * File:   TestCase.cpp
 * Author: maxim
 * 
 * Created on 12 Февраль 2014 г., 13:51
 */

#include "TestGen/TestCase.h"

namespace borealis {

TestCase::TestCase(const std::unordered_map<Term::Ptr, Term::Ptr> & testCase) :
        testCase(testCase){}

void TestCase::addArgument(const Term::Ptr arg, const Term::Ptr value) {
    testCase.emplace(arg, value);
}

const Term::Ptr TestCase::getValue(const Term::Ptr arg) const {
    //return testCase.at(arg);
    std::cout << "===============\nWANT:" << arg->getName() << "\n===============\n";
    for (const auto & a: testCase) {
        if (a.first->getName() == arg->getName()) {
            std::cout << "===============\n" << a.second << "\n===============\n";
            return a.second;
        }
    }
    std::cout << "===============\nNO:" << arg->getName() << "\n===============\n";
}

TestCase::~TestCase() {
}

} /* namespace borealis */
