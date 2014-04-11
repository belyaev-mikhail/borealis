/*
 * cunit.h
 *
 *  Created on: Apr 1, 2014
 *      Author: gagarski
 */

#ifndef CUNIT_H_
#define CUNITMODULE_H_

#include <iostream>
#include "Passes/TestGeneration/TestManager.h"
#include "TestGen/PrototypesInfo.h"
#include "Passes/TestGeneration/TestDumpPass.h"

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
    CUnitModule(const TestMap& testMap,
              const SlotTrackerPass& stp, const MetaInfoTracker& mit,
              const FunctionAnnotationTracker& fat, PrototypesLocation& protoLoc,
              const std::string& moduleName,
              const std::string& baseDirectory):
                  testMap(testMap), stp(stp), mit(mit), fat(fat), prototypes(protoLoc.provide()),
                  moduleName(moduleName), baseDirectory(baseDirectory) {};

    friend std::ostream& operator<<(std::ostream& os, const CUnitModule& test);

private:
    void generateHeader(std::ostream& os) const;

    const TestMap& testMap;
    const SlotTrackerPass& stp;
    const MetaInfoTracker& mit;
    const FunctionAnnotationTracker& fat;
    PrototypesInfo prototypes;
    const std::string& moduleName;
    const std::string& baseDirectory;

};

} /* namespace util */

} /* namespace borealis */


#endif /* CUNIT_H_ */
