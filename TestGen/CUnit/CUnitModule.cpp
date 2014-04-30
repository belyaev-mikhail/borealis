#include <iostream>

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/PathV2.h>

#include "Passes/Defect/DefectManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/TestGeneration/TestManager.h"
#include "Passes/Tracker/MetaInfoTracker.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "Passes/Tracker/FunctionAnnotationTracker.h"
#include "TestGen/CUnit/CUnitModule.h"
#include "TestGen/CUnit/CUnitSuite.h"
#include "TestGen/CUnit/util.hpp"
#include "Util/filename_utils.h"

#include "State/Transformer/ContractStupidifier.h"
#include "Util/filename_utils.h"

namespace borealis {

namespace util {


void CUnitModule::generateHeader(std::ostream& os) const {
    os << "#include <CUnit/Basic.h>\n\n";
    auto funcsView = util::view(testMap.begin(), testMap.end())
        .map([](decltype(*testMap.begin()) pair) { return pair.first; });
    auto includes = util::getIncludesForFunctions(
            funcsView.begin(), funcsView.end(), prototypes);

    util::writeIncludes(includes.begin(), includes.end(), os, baseDirectory, moduleName);
    os << "\n";
    if (TestDumpPass::includeInMakefile()) {
        os << "#include \"" << TestDumpPass::oracleHeaderFilename(moduleName) << "\"\n";
    } else if (TestDumpPass::absoluteInclude()) {
        os << "#include \""
           << util::getAbsolutePath(baseDirectory,
                   llvm::StringRef(TestDumpPass::oracleHeaderPath(moduleName))) << "\"\n";
    } else {
        os << "#include \""
           << util::getRelativePath(baseDirectory,
                   llvm::StringRef(TestDumpPass::oracleHeaderPath(moduleName)),
                   llvm::StringRef(TestDumpPass::filePathForModule(moduleName)))
           << "\"\n";
    }
    for (const auto& pair: testMap) {
        auto testSuite = pair.second;
        if (testSuite != nullptr) {
            os << CUnitSuitePrototype(*testSuite, mit, &prototypes);
        }
    }
    os << "\n";
}

std::ostream& operator<<(std::ostream& os, const CUnitModule& test) {
    test.generateHeader(os);

    for (auto& pair: test.testMap) {
        auto* f = const_cast<llvm::Function*>(pair.first);
        auto* st = test.stp.getSlotTracker(*f);

        auto fn = FactoryNest(st);

        auto testSuite = pair.second;

        if (testSuite != nullptr) {

            ContractStupidifier cs{
                util::view(f->arg_begin(), f->arg_end())
                     .map([&](llvm::Argument& a){
                          return fn.Term->getArgumentTerm(&a);
                      })
                     .toVector(),
                fn.Term->getValueTerm(fn.Type->getUnknownType(), testSuite->getResultVariableName()),
                fn
            };


            auto oracle = util::viewContainer(test.fat.getAnnotations(*f))
                         .map(llvm::dyn_caster<EnsuresAnnotation>{})
                         .filter()
                         .map([&](const EnsuresAnnotation* anno){
                             return cs.transform(anno->getTerm());
                          })
                         .toVector();
            os << CUnitSuiteDefinitions(*testSuite, fn, test.mit, oracle);

        }
    }

    os << "int run" << util::capitalize(test.moduleName) << "Test(void) {\n";
    os << "    if (CUE_SUCCESS != CU_initialize_registry())\n"
       << "        return CU_get_error();\n";

    for (auto& f: test.testMap) {
        auto testSuite = f.second;
        os << CUnitSuiteActivation(*testSuite);
    }

    os << "    CU_basic_run_tests();\n";
    os << "    CU_cleanup_registry();\n";
    os << "    return 0;\n";
    os << "}\n";

    return os;
}

} /* namespace util */

} /* namespace borealis */
