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

namespace borealis {

TestCase::TestCase(const TestCase::TermMap & testCase) :
        testCase(testCase){}

void TestCase::addArgument(const Term::Ptr arg, const Term::Ptr value) {
    testCase.emplace(arg, value);
}

static std::string formatToType(Term::Ptr trm, borealis::DIType type) {
    while(DIAlias alias = type) type = alias.getOriginal();

    if(auto integer = llvm::dyn_cast<OpaqueIntConstantTerm>(trm)) {
        llvm::APInt app{ type.getSizeInBits(), integer->getValue(), !type.isUnsignedDIType() };
        return app.toString(10, !type.isUnsignedDIType());
    }
    return trm->getName();
}

void TestCase::generateTest(std::ostream& outStream, const llvm::Function* function,
    FactoryNest fn, MetaInfoTracker* mit, int id, const std::vector<Term::Ptr>& oracle) const {
    using namespace borealis::util;

    outStream << "void " << getTestName(function, id) << "(void) {\n";
    std::string args;
    for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
        auto arg_ = const_cast<llvm::Argument *>(&(*arg));
        auto argTerm = fn.Term->getArgumentTerm(arg_);
        auto type = mit->locate(arg_).front().type;
        outStream << "    "
                  << getCType(type, CTypeModifiersPolicy::DISCARD)
                  << " " << arg->getName() << " = " << formatToType(getValue(argTerm), type) << ";\n";
        args += arg->getName().str() + ", ";
    }
    args.erase(args.end() - 2, args.end());
    auto f = const_cast<llvm::Function *>(function);
    outStream << "    "
              << getCType(mit->locate(f).front().type, CTypeModifiersPolicy::DISCARD)
              << " res = " << function->getName() << "(" << args << ");\n";

    for(auto& check : oracle) {
        outStream << "    " << "CU_ASSERT(" << check->getName() << ");\n";
    }

    outStream << "}\n\n";
}

void TestCase::activateTest(std::ostream & outStream, const TestSuite & suite, int id) const {
    outStream << "    if (CU_add_test(" << suite.getSuiteName() << ", \"Test #"
              << id << " for " << suite.getFunctionName() << "\", "
              << getTestName(suite.getFunctionName(), id) << ") == NULL) {\n"
              << "        CU_cleanup_registry();\n"
              << "        return CU_get_error();\n"
              << "    }\n";
}


std::string TestCase::getTestName(const llvm::Function * function, int id) const {
    return getTestName(function->getName(), id);
}

std::string TestCase::getTestName(llvm::StringRef functionName, int id) const {
    std::string name = functionName.str();
    name[0] = std::toupper(name[0]);
    return "test" + name + "_" + util::toString(id);
}

const Term::Ptr TestCase::getValue(const Term::Ptr arg) const {
    return testCase.at(arg);
}

} /* namespace borealis */
