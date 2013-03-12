/*
 * DefectManager.h
 *
 *  Created on: Jan 24, 2013
 *      Author: ice-phoenix
 */

#ifndef DEFECTMANAGER_H_
#define DEFECTMANAGER_H_

#include <llvm/Instructions.h>
#include <llvm/Pass.h>

#include <map>
#include <set>
#include <tuple>

#include "Logging/logger.hpp"
#include "Util/locations.h"

namespace borealis {

enum class DefectType {
    INI_03,
    REQ_01,
    ENS_01
};

struct DefectSummary {
    std::string type;
    std::string description;
};

template<class Streamer>
Streamer& operator<<(Streamer& str, const DefectSummary& ds) {
    // this is generally fucked up
    return static_cast<Streamer&>(str << "\"" << ds.type << "\": " << ds.description);
}

const std::map<DefectType, const DefectSummary> DefectTypeNames = {
    { DefectType::INI_03, { "INI-03", "Dereferencing a nullptr" } },
    { DefectType::REQ_01, { "REQ-01", "Requires contract check failed" } },
    { DefectType::ENS_01, { "ENS-01", "Ensures contract check failed" } }
};

class DefectManager:
        public llvm::ModulePass,
        public borealis::logging::ClassLevelLogging<DefectManager> {

public:

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("defect-manager")
#include "Util/unmacros.h"

    typedef std::set< std::pair<DefectType, const Locus> > DefectData;
    typedef DefectData::value_type DefectDataEntry;

    static char ID;

    DefectManager();
    virtual bool runOnModule(llvm::Module&) { return false; }
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const;
    virtual ~DefectManager() {};

    void addDefect(DefectType type, llvm::Instruction* where);

    virtual void print(llvm::raw_ostream&, const llvm::Module*) const;

private:

    static DefectData data;

};

} /* namespace borealis */

#endif /* DEFECTMANAGER_H_ */
