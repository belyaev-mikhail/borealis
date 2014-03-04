/* 
 * File:   CUnitDumperPass.h
 * Author: maxim
 *
 * Created on 18 Февраль 2014 г., 17:16
 */

#ifndef CUNITDUMPERPASS_H
#define	CUNITDUMPERPASS_H

#include <fstream>
#include <llvm/Pass.h>

#include "Factory/Nest.h"
#include "Logging/logger.hpp"
#include "Passes/Defect/DefectManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/TestGeneration/TestManager.h"
#include "Passes/Tracker/MetaInfoTracker.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "Util/passes.hpp"

namespace borealis {

class CUnitDumperPass :
        public llvm::ModulePass,
        public borealis::logging::ClassLevelLogging<TestGenerationPass> {
public:

    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("cunit-dumper")
#include "Util/unmacros.h"

    CUnitDumperPass();
    
    virtual bool runOnModule(llvm::Module & M) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage & AU) const override;
    virtual ~CUnitDumperPass();
    
private:
    std::ofstream testFile;
    
    void generateHeader();
};

} /* namespace borealis */

#endif	/* CUNITDUMPERPASS_H */

