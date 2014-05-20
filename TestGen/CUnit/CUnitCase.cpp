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

static std::string formatToType(Term::Ptr trm, borealis::DIType type) {
    while(DIAlias alias = type) type = alias.getOriginal();

    if(auto integer = llvm::dyn_cast<OpaqueIntConstantTerm>(trm)) {
        llvm::APInt app{ type.getSizeInBits(), integer->getValue(), !type.isUnsignedDIType() };
        return app.toString(10, !type.isUnsignedDIType());
    }
    return trm->getName();
}

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
    auto* function = suite.getFunction();
    os << "void " << cs.getTestName(function, definition.id) << "(void) {\n";
    
    auto fi = definition.fip.getFunctionInfo(function);
    
    std::string args;
    if (fi.getArgsCount() > 0) {
        for (auto arg : fi) {
            os << "    "
               << getCType(arg.type, CTypeModifiersPolicy::DISCARD)
               << " " << arg.name << " = " << formatToType(cs.getValue(arg.term), arg.type) << ";\n";
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
