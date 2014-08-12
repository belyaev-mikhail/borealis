/*
 * CUnitCase.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: gagarski
 */

#include <string>

#include "Passes/TestGeneration/TestDumpPass.h"
#include "Term/Term.h"
#include "TestGen/CUnit/CUnitCase.h"
#include "TestGen/Util/c_types.h"
namespace borealis {
namespace util {

std::ostream& operator<<(std::ostream& os, const CUnitCaseActivation& activation) {
    auto& cs = activation.cs;
    auto& suite = activation.suite;
    os << "    if (CU_add_test(" << suite.getSuiteName() << ", \"Test #"
       << activation.id << " for " << suite.getFunctionName() << "\", "
       << cs.getTestName(activation.suite.getFunctionName(), activation.id) << ") == NULL) {\n"
       << "        CU_cleanup_registry();\n"
       << "        return CU_get_error();\n"
       << "    }\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CUnitCaseDefinition& definition) {
    auto& cs = definition.cs;
    auto& suite = definition.suite;
    const auto& fi = *suite.getFunctionInfo();
    
    if (fi.isStub()) {
        return os;
    }
    
    auto* function = fi.getFunction();
    
    if (fi.hasPtrArgs()) {
        definition.ts.addStubTest(definition.suite.getFunction());
        os << "// Has some stubs\n";
    } else {
        definition.ts.addGoodTest(definition.suite.getFunction());
    }
    
    os << "void " << cs.getTestName(function, definition.id) << "(void) {\n";
    
    std::string args;
    if (fi.getArgsCount() > 0) {
        for (const auto& arg : fi) {
            os << "    "
               << getCType(arg.type, CTypeModifiersPolicy::DISCARD)
               << " " << arg.name << " = " << arg.getValue(cs) << ";\n";
            args += arg.name + ", ";
        }
        args.erase(args.end() - 2, args.end());
    }
    auto resultName = TestDumpPass::getResultNameForFunction(function);
    os << "    "
       << getCType(fi.getReturnType(), CTypeModifiersPolicy::DISCARD)
       << " " << resultName << " = " << fi.getName()
       << "(" << args << ");\n";

    for(auto& check : definition.oracle) {
        os << "    " << "CU_ASSERT(" << check->getName() << ");\n";
    }
    if (TestDumpPass::insertUserOraclesCall()) {
        os << "    " << "CU_ASSERT(";
        os << function->getName() + "Oracle" << "(" << args << ", " << resultName << ")";
        os << ");\n";
    }

    os << "}\n\n";
    return os;
}
} /* namespace util */
} /* namespace borealis */
