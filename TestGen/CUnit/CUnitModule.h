/*
 * cunit.h
 *
 *  Created on: Apr 1, 2014
 *      Author: gagarski
 */

#ifndef CUNIT_H_
#define CUNIT_H_

#include <iostream>
#include "Passes/TestGeneration/TestManager.h"
#include "TestGen/PrototypesInfo.h"
#include "Passes/TestGeneration/TestDumpPass.h"

namespace borealis {

namespace util {

class CUnitModule {

public:
    typedef std::shared_ptr<CUnitModule> Ptr;
    typedef DataProvider<PrototypesInfo> prototypesLocation;
    typedef std::unordered_map<const llvm::Function*,
            TestSuite::Ptr> TestMap;
    typedef std::shared_ptr<TestMap> TestMapPtr;

    CUnitModule() = delete;
    CUnitModule(const CUnitModule& orig) = default;
    CUnitModule(CUnitModule&& orig) = default;
    CUnitModule(TestMap& testMap,
              SlotTrackerPass& stp, MetaInfoTracker& mit,
              FunctionAnnotationTracker& fat, prototypesLocation& protoLoc,
              llvm::StringRef baseDirectory, llvm::StringRef moduleName,
              llvm::StringRef filePath, bool absoluteInclude);

    friend std::ostream& operator<<(std::ostream& os, const CUnitModule& test);

private:
    void generateHeader(std::ostream& os) const;

    TestMap& testMap;
    llvm::StringRef moduleName;
    llvm::StringRef filePath;
    llvm::StringRef baseDirectory;
    PrototypesInfo prototypes;
    SlotTrackerPass& stp;
    MetaInfoTracker& mit;
    FunctionAnnotationTracker& fat;
    bool absoluteInclude;

};

} /* namespace util */

} /* namespace borealis */


#endif /* CUNIT_H_ */
