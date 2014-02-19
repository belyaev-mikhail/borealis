/* 
 * File:   TestSuite.cpp
 * Author: maxim
 * 
 * Created on 12 Февраль 2014 г., 13:50
 */

#include "TestGen/TestSuite.h"

namespace borealis {

TestSuite::TestSuite(llvm::Function * f) : function(f) {}

TestSuite::TestSuite(llvm::Function * f, const std::vector<TestCase> & tests) :
        function(f), tests(tests) {}

void TestSuite::addTestCase(const TestCase & testCase) {
    tests.push_back(testCase);
}

void TestSuite::generateTest(std::ostream & outStream, FactoryNest fn) const {
    outStream << "void test" << function->getName().upper() << "(void) {\n";
    for (const auto & t: tests) {
        outStream << "    " << function->getName() << "(";
        std::string args;
        for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
            auto argTerm = fn.Term->getArgumentTerm(&(*arg));
            args += t.getValue(argTerm)->getName() + ", ";
        }
        args.erase(args.end() - 2, args.end());
        outStream << args << ");\n";
    }
    outStream << "}\n\n";
}

TestSuite::~TestSuite() {
}

} /* namespace borealis */