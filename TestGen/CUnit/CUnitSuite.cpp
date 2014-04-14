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
    std::string args;
    if (!function->arg_empty()) {
        for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
            args += util::getCType(
                prototype.mit.locate(const_cast<llvm::Argument*>(&(*arg))).front().type,
                util::CTypeModifiersPolicy::KEEP
            );
            args += ", ";
        }
        args.erase(args.end() - 2, args.end());
    }
    os << util::getCType(
        prototype.mit.locate(const_cast<llvm::Function *>(function)).front().type,
        util::CTypeModifiersPolicy::KEEP
    );
    os << " " << function->getName() << "(" << args << ");\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CUnitSuiteDefinitions& defs) {
    int i = 0;
    for (auto& test: defs.suite) {
        os << CUnitCaseDefinition(test, defs.suite, defs.fn, defs.mit, i++,
                defs.oracle, defs.suite.getResultVariableName());
    }
    return os;
}

} /* namespace util */
} /* namespace borealis */
