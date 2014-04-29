/*
 * TestGenerationPass.cpp
 *
 *  Created on: Jan 27, 2014
 *      Author: sam
 */

#include <llvm/Support/InstIterator.h>
#include <llvm/Support/CFG.h>

#include "Codegen/intrinsics_manager.h"
#include "Passes/TestGeneration/TestGenerationPass.h"
#include "Passes/Tracker/SlotTrackerPass.h"
#include "SMT/MathSAT/Solver.h"
#include "SMT/Z3/Solver.h"
#include "TestGen/TestSuite.h"
#include "TestGen/util.h"
#include "Util/macros.h"
#include "Util/util.h"

namespace borealis {



TestGenerationPass::TestGenerationPass() : ProxyFunctionPass(ID) {}
TestGenerationPass::TestGenerationPass(llvm::Pass* pass) : ProxyFunctionPass(ID, pass) {}

void TestGenerationPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();

    AUX<FunctionManager>::addRequiredTransitive(AU);
    AUX<PredicateStateAnalysis>::addRequiredTransitive(AU);
    AUX<SlotTrackerPass>::addRequiredTransitive(AU);
	AUX<TestManager>::addRequiredTransitive(AU);
}

TestCase::Ptr TestGenerationPass::testForInst(llvm::Function& F,
                                     llvm::Instruction* inst,
                                     const std::vector<Term::Ptr>& args) {

    auto fMemId = FM->getMemoryStart(&F);

#if defined USE_MATHSAT_SOLVER
    MathSAT::ExprFactory ef;
    MathSAT::Solver s(ef, fMemId);
#else
    Z3::ExprFactory ef;
    Z3::Solver s(ef, fMemId);
#endif

    auto state = PSA->getInstructionState(inst);

    std::string blockName = inst->getParent()->getName();
    dbgs() << "State for block " << blockName << endl
           << state << endl;

    auto smtTest = s.generateTest(state, args);
    if (smtTest.empty()) {
        return nullptr;
    }

    TestCase::Ptr testCase(new TestCase());

    std::string testStr = "test case for block ";
    testStr += blockName;
    testStr += ": \n";
    for (const auto& testValue : smtTest) {
        testStr += testValue.first->getName() + " = " +
                   testValue.second->getName() + "\n";
        testCase->addArgument(testValue.first, testValue.second);
    }
    dbgs() << testStr;
    return testCase;
}

// check if a basic block is
static bool isInterestring(const llvm::BasicBlock* bb) {
    return util::view(llvm::pred_begin(bb), llvm::pred_end(bb))
          .any_of([](const llvm::BasicBlock* pred) -> bool{
                if(auto&& branch = llvm::dyn_cast<llvm::BranchInst>(pred->getTerminator()))
                    if(branch->isConditional()) return true;
                return false;
           });
}

bool TestGenerationPass::runOnFunction(llvm::Function& F) {

    if (util::shouldSkipTest(&F))
        return false;

    FM = &GetAnalysis<FunctionManager>::doit(this, F);
    PSA = &GetAnalysis<PredicateStateAnalysis>::doit(this, F);

    auto* st = GetAnalysis<SlotTrackerPass>::doit(this, F).getSlotTracker(F);
    FN = FactoryNest(st);
    
    TM = &GetAnalysis<TestManager>::doit(this, F);

    dbgs() << "name: " << F.getName() << endl;

    TestSuite::Ptr testSuite(new TestSuite(&F));

    std::vector<Term::Ptr> args;
    args.reserve(F.arg_size());
    for (auto& arg : util::view(F.arg_begin(), F.arg_end())) {
        args.push_back(FN.Term->getArgumentTerm(&arg));
    }

    // FIXME For function with only one BasicBlock generate test for first instruction.
    if (F.getBasicBlockList().size() == 1) {
        auto testCase = testForInst(F, &*(llvm::inst_begin(F)), args);
        if (testCase != nullptr)
            testSuite->addTestCase(*testCase);
    } else {
        for (auto&& BB : util::tail(F)) {
            if(!isInterestring(&BB)) continue;

            auto testCase = testForInst(F, &*(BB.begin()), args);
            if (testCase != nullptr)
                testSuite->addTestCase(*testCase);
        }
    }

    TM->update(&F, testSuite);
    return false;
}

TestGenerationPass::~TestGenerationPass() {}

char TestGenerationPass::ID;
static RegisterPass<TestGenerationPass>
X("test-generation", "Pass that generates unit tests.");

#include "Util/unmacros.h"

} /* namespace borealis */
