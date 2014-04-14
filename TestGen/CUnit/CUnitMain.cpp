#include <llvm/Analysis/DebugInfo.h>
#include <llvm/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/PathV2.h>

#include "TestGen/CUnit/CUnitMain.h"
#include "Util/filename_utils.h"
#include "Util/util.h"

namespace borealis {
namespace util {
std::ostream& operator<<(std::ostream& os, const CUnitMain& main) {
    os << "#include <CUnit/Basic.h>\n\n";
    os << "#include \"tests.h\"\n\n";

    os << "int main() {\n";
    os << "    CU_basic_set_mode(CU_BRM_VERBOSE);\n";
    auto* CUs = main.module.getNamedMetadata("llvm.dbg.cu");
    for (unsigned i = 0; i < CUs->getNumOperands(); i++) {
        llvm::DICompileUnit cu(CUs->getOperand(i));
        auto cuName = llvm::sys::path::stem(cu.getFilename());
        os << "    run" << util::capitalize(cuName) << "Test();\n";
    }
    os << "}\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CUnitHeader& hdr) {
    auto* CUs = hdr.module.getNamedMetadata("llvm.dbg.cu");
    auto includeGuard =  util::toUpperCase(hdr.fileName);
    includeGuard = "_" + util::replace(".", "_", includeGuard) + "_";
    os << "#ifndef " << includeGuard << "\n";
    os << "#define " << includeGuard << "\n";
    for (unsigned i = 0; i < CUs->getNumOperands(); i++) {
        llvm::DICompileUnit cu(CUs->getOperand(i));
        auto cuName = llvm::sys::path::stem(cu.getFilename());
        os << "int run" << util::capitalize(cuName) << "Test(void);\n";
    }
    os << "#endif /* " + includeGuard + " */\n";
    return os;
}

} /* namespace util */
} /* namespace borealis */
