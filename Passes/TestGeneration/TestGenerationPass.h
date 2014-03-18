/*
 * TestGenerationPass.h
 *
 *  Created on: Jan 27, 2014
 *      Author: sam
 */

#ifndef TESTGENERATIONPASS_H_
#define TESTGENERATIONPASS_H_

#include <llvm/Pass.h>

#include "Factory/Nest.h"
#include "Logging/logger.hpp"
#include "Passes/Manager/FunctionManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"
#include "Passes/Util/ProxyFunctionPass.h"
#include "TestGen/TestCase.h"
#include "Util/passes.hpp"

namespace borealis {

class TestGenerationPass :
        public ProxyFunctionPass,
        public borealis::logging::ClassLevelLogging<TestGenerationPass>,
        public ShouldBeModularized {

public:

    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("test-generation")
#include "Util/unmacros.h"

    TestGenerationPass();
    TestGenerationPass(llvm::Pass* pass);
    virtual bool runOnFunction(llvm::Function& F) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
    virtual ~TestGenerationPass();

private:

    FunctionManager* FM;
    PredicateStateAnalysis* PSA;
    FactoryNest FN;

    TestCase::Ptr testForInst(llvm::Function& F,
                     llvm::Instruction* inst,
                     const std::vector<Term::Ptr>& args);

};

} /* namespace borealis */


#endif /* TESTGENERATIONPASS_H_ */
