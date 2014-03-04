/* 
 * File:   TestManager.cpp
 * Author: maxim
 * 
 * Created on 3 Март 2014 г., 16:51
 */

#include "TestManager.h"
#include "Util/passes.hpp"

#include "Util/macros.h"

namespace borealis {

TestManager::TestManager() : llvm::ImmutablePass(ID) {}

void TestManager::getAnalysisUsage(llvm::AnalysisUsage & AU) const {
    AU.setPreservesAll();
}

void TestManager::put(const llvm::Function * F, TestSuite::Ptr tests) {
    using borealis::util::containsKey;

    ASSERT(!containsKey(functionTests, F),
           "Attempt to register function " + F->getName().str() + " twice")

    functionTests[F] = tests;
}

void TestManager::update(const llvm::Function * F, TestSuite::Ptr tests) {
    using borealis::util::containsKey;

    dbgs() << "Updating function state for: " << F->getName().str() << endl;

    if (containsKey(functionTests, F)) {
        auto ts = TestSuite::Ptr{new TestSuite(*functionTests.at(F))};
        ts->addTestSuite(*tests);
        functionTests[F] = ts;
    } else {
        functionTests[F] = tests;
    }
}

TestSuite::Ptr TestManager::getTests(const llvm::Function * F) const {
    auto tests = functionTests.find(F);
    if (tests == functionTests.end()) {
        return nullptr;
    } else {
        return tests->second;
    }
}

char TestManager::ID;
static RegisterPass<TestManager>
X("test-manager", "Pass that manages function tests");

} /* namespace borealis */

#include "Util/unmacros.h"