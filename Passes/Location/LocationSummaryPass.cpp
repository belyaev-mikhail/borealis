/*
 * LocationSummaryPass.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: snowball
 */

#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/CommandLine.h>
#include <yaml-cpp/yaml.h>

#include <fstream>

#include "Passes/Location/LocationManager.h"
#include "Passes/Location/LocationSummaryPass.h"
#include "Passes/Tracker/SourceLocationTracker.h"
#include "Passes/Util/DataProvider.hpp"

#include "Util/passes.hpp"

namespace borealis {

typedef DataProvider<clang::SourceManager> DPSourceManager;

static llvm::cl::opt<bool>
DumpCoverage("dump-coverage", llvm::cl::init(false), llvm::cl::NotHidden,
  llvm::cl::desc("Dump analysis coverage"));

static llvm::cl::opt<std::string>
DumpCoverageFile("dump-coverage-file", llvm::cl::init("%s.coverage"), llvm::cl::NotHidden,
  llvm::cl::desc("Output file for analysis coverage"));

LocationSummaryPass::LocationSummaryPass(): ModulePass(ID) {}

void LocationSummaryPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AUX<SourceLocationTracker>::addRequiredTransitive(AU);
    AUX<LocationManager>::addRequiredTransitive(AU);
    AUX<DPSourceManager>::addRequiredTransitive(AU);
}

bool LocationSummaryPass::runOnModule(llvm::Module&) {
    auto& slt = GetAnalysis<SourceLocationTracker>::doit(this);
    auto& lm = GetAnalysis<LocationManager>::doit(this);
    auto& sm = GetAnalysis<DPSourceManager>::doit(this).provide();

    auto* mainFileEntry = sm.getFileEntryForID(sm.getMainFileID());

    if (DumpCoverage || !DumpCoverageFile.empty()) {

        util::replace("%s", mainFileEntry->getName(), DumpCoverageFile);

        auto& llvmLocs = lm.getLocations();

        std::map<std::string, std::set<std::string>> locMap;

        auto& normalLocs = locMap["normal"];
        for (const auto& v : llvmLocs) {
            auto& loc = slt.getLocFor(v);
            std::ostringstream oss;
            oss << "!SourceCodeLocation " << loc;
            normalLocs.insert(oss.str());
        }

        YAML::Emitter yaml;
        yaml << locMap;

        std::ofstream output(DumpCoverageFile);
        output << yaml.c_str();
        output.close();
    }

    return false;
}

char LocationSummaryPass::ID;
static RegisterPass<LocationSummaryPass>
X("location-summary", "Pass that outputs visited locations");

} /* namespace borealis */
