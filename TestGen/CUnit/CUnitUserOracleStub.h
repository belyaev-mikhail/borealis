/*
 * CUnitOracleModule.h
 *
 *  Created on: Apr 10, 2014
 *      Author: gagarski
 */

#ifndef CUNITUSERORACLESSTUB_H_
#define CUNITUSERORACLESSTUB_H_

#include <unistd.h>
#include <iostream>

#include <llvm/Function.h>

#include "Passes/TestGeneration/TestDumpPass.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "TestGen/CUnit/util.hpp"
#include "TestGen/SourceLocations.h"
#include "Util/filename_utils.h"

namespace borealis {

namespace util {

class CUnitUserOracleStubDefinition {
public:
    typedef std::shared_ptr<CUnitUserOracleStubDefinition> Ptr;
    CUnitUserOracleStubDefinition() = delete;
    CUnitUserOracleStubDefinition(const CUnitUserOracleStubDefinition& orig) = default;
    CUnitUserOracleStubDefinition(CUnitUserOracleStubDefinition&& orig) = default;
    CUnitUserOracleStubDefinition(const llvm::Function* function, const SlotTrackerPass& stp,
            const MetaInfoTracker& mit) :
        function(function), stp(stp), mit(mit) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubDefinition& stub);

private:
    const llvm::Function* function;
    const SlotTrackerPass& stp;
    const MetaInfoTracker& mit;
};

class CUnitUserOracleStubDecl {
public:
    typedef std::shared_ptr<CUnitUserOracleStubDecl> Ptr;
    CUnitUserOracleStubDecl() = delete;
    CUnitUserOracleStubDecl(const CUnitUserOracleStubDecl& orig) = default;
    CUnitUserOracleStubDecl(CUnitUserOracleStubDecl&& orig) = default;
    CUnitUserOracleStubDecl(const llvm::Function* function, const SlotTrackerPass& stp,
            const MetaInfoTracker& mit) :
        function(function), stp(stp), mit(mit) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubDecl& proto);

private:
    const llvm::Function* function;
    const SlotTrackerPass& stp;
    const MetaInfoTracker& mit;
};

class CUnitUserOracleStubProto {
public:
    typedef std::shared_ptr<CUnitUserOracleStubProto> Ptr;
    CUnitUserOracleStubProto() = delete;
    CUnitUserOracleStubProto(const CUnitUserOracleStubProto& orig) = default;
    CUnitUserOracleStubProto(CUnitUserOracleStubProto&& orig) = default;
    CUnitUserOracleStubProto(const llvm::Function* function, const SlotTrackerPass& stp,
            const MetaInfoTracker& mit) :
        function(function), stp(stp), mit(mit) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubProto& proto);

private:
    const llvm::Function* function;
    const SlotTrackerPass& stp;
    const MetaInfoTracker& mit;
};



class CUnitUserOracleStubModule {
public:
    typedef std::shared_ptr<CUnitUserOracleStubModule> Ptr;
    typedef std::unordered_set<const llvm::Function*> FuncList;
    typedef DataProvider<PrototypesInfo> prototypesLocation;
    CUnitUserOracleStubModule() = delete;
    CUnitUserOracleStubModule(const CUnitUserOracleStubModule& orig) = default;
    CUnitUserOracleStubModule(CUnitUserOracleStubModule&& orig) = default;
    CUnitUserOracleStubModule(const FuncList& funcs, const SlotTrackerPass& stp,
            const MetaInfoTracker& mit, prototypesLocation& protoLoc,
            const std::string& moduleName, const std::string& baseDirectory) :
                funcs(funcs), stp(stp), mit(mit), moduleName(moduleName),
                baseDirectory(baseDirectory) {
        prototypes = protoLoc.provide();
    };
    bool addToFile(const std::string& fileName, LocationAnalyseResult& oldLocs);
    friend std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubModule& module);
    template<class ToInsert, class Unit>
    friend bool updateOracleFile(
            Unit& module,
            const std::string& fileName, LocationAnalyseResult& oldLocs);

private:
    void writeIncludes(std::ostream& os, const IncludesLocationsInFile* oldLocs) const;
    const FuncList& funcs;
    const SlotTrackerPass& stp;
    const MetaInfoTracker& mit;
    PrototypesInfo prototypes;
    const std::string& moduleName;
    const std::string& baseDirectory;
};

class CUnitUserOracleStubHeader {
public:
    typedef std::shared_ptr<CUnitUserOracleStubHeader> Ptr;
    typedef std::unordered_set<const llvm::Function*> FuncList;
    typedef DataProvider<PrototypesInfo> prototypesLocation;
    CUnitUserOracleStubHeader() = delete;
    CUnitUserOracleStubHeader(const CUnitUserOracleStubHeader& orig) = default;
    CUnitUserOracleStubHeader(CUnitUserOracleStubHeader&& orig) = default;
    CUnitUserOracleStubHeader(const FuncList& funcs, const SlotTrackerPass& stp, const MetaInfoTracker& mit,
            prototypesLocation& protoLoc, const std::string& moduleName, const std::string& baseDirectory) :
                funcs(funcs), stp(stp), mit(mit), moduleName(moduleName),
                baseDirectory(baseDirectory) {
        prototypes = protoLoc.provide();
    };
    bool addToFile(const std::string& fileName, LocationAnalyseResult& oldLocs);
    friend std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubHeader& hdr);
    template<class ToInsert, class Unit>
    friend bool updateOracleFile(
            Unit& hdr,
            const std::string& fileName, LocationAnalyseResult& oldLocs);

private:
    const FuncList& funcs;
    const SlotTrackerPass& stp;
    const MetaInfoTracker& mit;
    const std::string& moduleName;
    const std::string& baseDirectory;
    PrototypesInfo prototypes;

};
} /* namespace util */

} /* namespace borealis */

#endif /* CUNITUSERORACLESSTUB_H_ */
