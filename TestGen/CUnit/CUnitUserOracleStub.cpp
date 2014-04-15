/*
 * CUnitOraclesStubs.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: gagarski
 */

#include "Passes/Tracker/SlotTrackerPass.h"
#include "TestGen/CUnit/CUnitUserOracleStub.h"
#include "TestGen/Util/c_types.h"

namespace borealis {

namespace util {

std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubDecl& decl) {
    auto* function = decl.function;
    auto* f = const_cast<llvm::Function*>(function);
    auto* st = decl.stp.getSlotTracker(*f);
    auto fn = FactoryNest(st);
    os << "int " << function->getName() << "Oracle(";
    std::string args;
    if (!function->arg_empty()) {
        for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
            auto arg_ = const_cast<llvm::Argument *>(&(*arg));
            auto argTerm = fn.Term->getArgumentTerm(arg_);
            auto type = decl.mit.locate(arg_).front().type;
            args += getCType(type, CTypeModifiersPolicy::DISCARD);
            args += " ";
            args += arg->getName();
            args += ", ";

        }
    }
    args += getCType(decl.mit.locate(f).front().type, CTypeModifiersPolicy::DISCARD);
    args += " ";
    args += TestDumpPass::getResultNameForFunction(f);
    os << args;
    os << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubDefinition& stub) {
    os << CUnitUserOracleStubDecl(stub.function, stub.stp, stub.mit);
    os << " {\n";
    os << "    // Put your oracle code here.\n";
    os << "    return 1;\n";
    os << "}\n\n";
    return os;
}



std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubProto& proto) {
    os << CUnitUserOracleStubDecl(proto.function, proto.stp, proto.mit);
    os << ";\n";
    return os;
}
std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubModule& module) {
    std::unordered_set<std::string> userIncludes;
    for (const auto* f : module.funcs) {
        auto loc = module.prototypes.locations.find(f->getName());
        if (loc != module.prototypes.locations.end()) {
            userIncludes.insert(loc->second);
        }
    }
    std::vector<std::string> includes(userIncludes.begin(), userIncludes.end());
    sort(includes.begin(), includes.end());
    // including files where functions are defined is necessary for forward declarating
    // types used in parameters
    for (const auto& i: includes) {
        if (TestDumpPass::includeInMakefile()) {
            os << "#include \"" << i << "\"\n";
        } else if (TestDumpPass::absoluteInclude()) {
            os << "#include \"" << util::getAbsolutePath(module.baseDirectory,
                    llvm::StringRef(i))
               << "\"\n";
        } else {
            os << "#include \"" << util::getRelativePath(module.baseDirectory,
                    llvm::StringRef(i),
                    llvm::StringRef(TestDumpPass::oracleHeaderPath(module.moduleName)))
               << "\"\n";
        }
    }
    os << "\n";
    for(auto& f: module.funcs) {
        os << CUnitUserOracleStubDefinition(f, module.stp, module.mit);
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubHeader& hdr) {
    std::unordered_set<std::string> userIncludes;
    for (const auto* f : hdr.funcs) {
        auto loc = hdr.prototypes.locations.find(f->getName());
        if (loc != hdr.prototypes.locations.end()) {
            userIncludes.insert(loc->second);
        }
    }
    std::vector<std::string> includes(userIncludes.begin(), userIncludes.end());
    sort(includes.begin(), includes.end());
    // including files where functions are defined is necessary for forward declarating
    // types used in parameters
    for (const auto& i: includes) {
        if (TestDumpPass::includeInMakefile()) {
            os << "#include \"" << i << "\"\n";
        } else if (TestDumpPass::absoluteInclude()) {
            os << "#include \""
               << util::getAbsolutePath(hdr.baseDirectory,
                       llvm::StringRef(i)) << "\"\n";
        } else {
            os << "#include \""
               << util::getRelativePath(hdr.baseDirectory,
                       llvm::StringRef(i), llvm::StringRef(TestDumpPass::oracleHeaderPath(hdr.moduleName)))
               << "\"\n";
        }
    }
    os << "\n";
    // auto includeGuard =  util::toUpperCase(TestDumpPass::oracleHeaderPath(hdr.moduleName));
    // includeGuard = "_" + util::replace(".", "_", includeGuard) + "_";
    // os << "#ifndef " << includeGuard << "\n";
    // os << "#define " << includeGuard << "\n\n";
    // TODO
    // Adding new oracles protos
    for(auto& f: hdr.funcs) {
        os << CUnitUserOracleStubProto(f, hdr.stp, hdr.mit);
    }
    // os << "#endif /* " + includeGuard + " */\n";
    return os;
}

} /* namespace util */

} /* namespace borealis */
