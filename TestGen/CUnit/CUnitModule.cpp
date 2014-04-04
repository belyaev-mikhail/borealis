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
#include "Util/filename_utils.h"

#include "State/Transformer/ContractStupidifier.h"
#include "Util/filename_utils.h"

namespace borealis {

namespace util {

CUnitModule::CUnitModule(TestMap& testMap,
        SlotTrackerPass& stp, MetaInfoTracker& mit,
        FunctionAnnotationTracker& fat, prototypesLocation& protoLoc,
        llvm::StringRef baseDirectory, llvm::StringRef moduleName,
        llvm::StringRef filePath) :
            testMap(testMap), stp(stp), mit(mit), fat(fat) {
    prototypes = protoLoc.provide();
    this->baseDirectory = baseDirectory;
    this->moduleName = moduleName;
    this->filePath = filePath;

}

void CUnitModule::generateHeader(std::ostream& os) const {
    os << "#include <CUnit/Basic.h>\n\n";

    std::unordered_set<std::string> userIncludes;
    for (const auto& pair : testMap) {
        auto* f = pair.first;
        auto loc = prototypes.locations.find(f->getName());
        if (loc != prototypes.locations.end()) {
            userIncludes.insert(loc->second);
        }
    }
    std::vector<std::string> includes(userIncludes.begin(), userIncludes.end());
    sort(includes.begin(), includes.end());
    for (const auto& i: includes) {
        os << "#include \"" << util::getRelativePath(baseDirectory, llvm::StringRef(i), llvm::StringRef(filePath.str())) << "\"\n";
    }
    os << "\n";

    for (const auto& pair: testMap) {
        auto testSuite = pair.second;
        if (testSuite != nullptr) {
            testSuite->prototypeFunction(os, &mit, &prototypes);
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

            testSuite->generateTest(os, fn, &(test.mit), oracle);

        }
    }

    os << "int run" << util::capitalize(test.moduleName) << "Test(void) {\n";
    os << "    if (CUE_SUCCESS != CU_initialize_registry())\n"
       << "        return CU_get_error();\n";

    for (auto& f: test.testMap) {
        auto testSuite = f.second;
        testSuite->activateTest(os);
    }

    os << "    CU_basic_run_tests();\n";
    os << "    CU_cleanup_registry();\n";
    os << "    return 0;\n";
    os << "}\n";

    return os;
}

} /* namespace util */

} /* namespace borealis */
