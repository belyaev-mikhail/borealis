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
    std::string args;
    if (!function->arg_empty()) {
        for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
            auto arg_ = const_cast<llvm::Argument *>(&(*arg));
            auto argTerm = definition.fn.Term->getArgumentTerm(arg_);
            auto type = definition.mit.locate(arg_).front().type;
            os << "    "
               << getCType(type, CTypeModifiersPolicy::DISCARD)
               << " " << arg->getName() << " = " << formatToType(cs.getValue(argTerm), type) << ";\n";
            args += arg->getName().str() + ", ";
        }
        args.erase(args.end() - 2, args.end());
    }
    auto f = const_cast<llvm::Function *>(function);
    auto resultName = TestDumpPass::getResultNameForFunction(function);
    os << "    "
       << getCType(definition.mit.locate(f).front().type, CTypeModifiersPolicy::DISCARD)
       << " " << resultName << " = " << function->getName()
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
