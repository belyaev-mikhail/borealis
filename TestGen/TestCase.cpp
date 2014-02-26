/* 
 * File:   TestCase.cpp
 * Author: maxim
 * 
 * Created on 12 Февраль 2014 г., 13:51
 */

#include "TestGen/TestCase.h"
#include "TestGen/TestSuite.h"

namespace borealis {

TestCase::TestCase(const TestCase::TermMap & testCase) :
        testCase(testCase){}

void TestCase::addArgument(const Term::Ptr arg, const Term::Ptr value) {
    testCase.emplace(arg, value);
}

void TestCase::generateTest(std::ostream & outStream, const llvm::Function * function,  FactoryNest fn, int id) {
    this->id = id;
    outStream << "void " << getTestName(function) << "(void) {\n";
    outStream << "    " << function->getName() << "(";
    std::string args;
    for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
        auto argTerm = fn.Term->getArgumentTerm(&(*arg));
        args += getValue(argTerm)->getName() + ", ";
    }
    args.erase(args.end() - 2, args.end());
    outStream << args << ");\n";
    outStream << "}\n\n";
}

void TestCase::activateTest(std::ostream & outStream, const TestSuite & suite) const {
    outStream << "    if (CU_add_test(" << suite.getSuiteName() << ", \"Test #" << id << " for " << suite.getFunctionName() << "\", " << getTestName(suite.getFunctionName()) << ") == NULL) {\n"
              << "        CU_cleanup_registry();\n"
              << "        return CU_get_error();\n"
              << "    }\n";
}


std::string TestCase::getTestName(const llvm::Function * function) const {
    return getTestName(function->getName());
}

std::string TestCase::getTestName(llvm::StringRef functionName) const {
    return "test" + functionName.upper() + util::toString(id);
}

const Term::Ptr TestCase::getValue(const Term::Ptr arg) const {
    return testCase.at(arg);
}

} /* namespace borealis */
