/*
 * cunit.h
 *
 *  Created on: Apr 1, 2014
 *      Author: gagarski
 */

#ifndef CUNITMODULE_H_
#define CUNITMODULE_H_

#include "Passes/TestGeneration/FunctionInfoPass.h"
#include "Passes/TestGeneration/TestDumpPass.h"
#include "Passes/TestGeneration/TestManager.h"
#include "TestGen/FunctionInfo.h"
#include "TestGen/PrototypesInfo.h"

namespace borealis {

namespace util {

class CUnitModule {

public:
    typedef std::shared_ptr<CUnitModule> Ptr;
    typedef DataProvider<PrototypesInfo> PrototypesLocation;
    typedef std::unordered_map<const llvm::Function*,
            TestSuite::Ptr> TestMap;
    typedef std::shared_ptr<TestMap> TestMapPtr;

    CUnitModule() = delete;
    CUnitModule(const CUnitModule& orig) = default;
    CUnitModule(CUnitModule&& orig) = default;
    CUnitModule(const TestMap& testMap, const FunctionInfoPass& fip,
              const SlotTrackerPass& stp, const FunctionAnnotationTracker& fat,
              PrototypesLocation& protoLoc, const std::string& moduleName,
              const std::string& baseDirectory):
                  testMap(testMap), fip(fip), stp(stp), fat(fat),
                  prototypes(protoLoc.provide()), moduleName(moduleName), baseDirectory(baseDirectory) {};

    friend std::ostream& operator<<(std::ostream& os, const CUnitModule& test);

private:
    void generateHeader(std::ostream& os) const;

    const TestMap& testMap;
    const FunctionInfoPass& fip;
    const SlotTrackerPass& stp;
    const FunctionAnnotationTracker& fat;
    PrototypesInfo prototypes;
    const std::string& moduleName;
    const std::string& baseDirectory;

};

} /* namespace util */

} /* namespace borealis */


#endif /* CUNITMODULE_H_ */
