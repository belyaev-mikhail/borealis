/*
 * CUnitSuite.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: gagarski
 */

#include "TestGen/TestSuite.h"
#include "TestGen/Util/c_types.h"
#include "TestGen/CUnit/CUnitCase.h"
#include "TestGen/CUnit/CUnitSuite.h"
#include "Passes/TestGeneration/FunctionInfoPass.h"

namespace borealis {
namespace util {

std::ostream& operator<<(std::ostream& os, const CUnitSuiteActivation& activation) {
    if (activation.suite.empty()) {
        return os;
    }
    os << "    CU_pSuite " << activation.suite.getSuiteName()
               << " = CU_add_suite(\"Suite for " << activation.suite.getFunctionName()
               << "\", NULL, NULL);\n"
       << "    if (" << activation.suite.getSuiteName() << " == NULL) {\n"
       << "        CU_cleanup_registry();\n"
       << "        return CU_get_error();\n"
       << "    }\n";
    int id = 0;
    for (auto& t: activation.suite) {
        os << CUnitCaseActivation(t, activation.suite, id++);
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const CUnitSuitePrototype& prototype) {
    auto* function = prototype.suite.getFunction();
    if (util::containsKey(prototype.prototypes->locations, function->getName().str())) {
        return os;
    }
    
    auto fi = prototype.fip.getFunctionInfo(function);
    
    std::string args;
    if (fi.getArgsCount() > 0) {
        for (const auto& arg : fi) {
            args += util::getCType(arg.type, util::CTypeModifiersPolicy::KEEP);
            args += ", ";
        }
        args.erase(args.end() - 2, args.end());
    }
    os << util::getCType(fi.getReturnType(), util::CTypeModifiersPolicy::KEEP);
    os << " " << fi.getName() << "(" << args << ");\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CUnitSuiteDefinitions& defs) {
    int i = 0;
    for (auto& test: defs.suite) {
        os << CUnitCaseDefinition(test, defs.suite, defs.fip, i++,
                defs.oracle);
    }
    return os;
}

} /* namespace util */
} /* namespace borealis */
