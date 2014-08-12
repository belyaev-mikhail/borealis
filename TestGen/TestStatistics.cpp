/* 
 * File:   TestStatistics.cpp
 * Author: maxim
 * 
 * Created on 11 Август 2014 г., 12:09
 */

#include <llvm/Support/InstIterator.h>

#include "TestGen/TestStatistics.h"
#include "Util/passes.hpp"

namespace borealis {

TestStatistics::TestStatistics() : llvm::ImmutablePass(ID) {}

void TestStatistics::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
}
    
void TestStatistics::addGoodTest(const llvm::Function* f) {
    auto fs = statistics.find(f);
    if (fs == statistics.end()) {
        statistics.emplace(f, TestStatistics::FunctionStatistics {1, 0, 0});
    } else {
        fs->second.good++;
    }
}

void TestStatistics::addUnknownTest(const llvm::Function* f) {
    auto fs = statistics.find(f);
    if (fs == statistics.end()) {
        statistics.emplace(f, TestStatistics::FunctionStatistics {0, 1, 0});
    } else {
        fs->second.unknown++;
    }
}

void TestStatistics::addStubTest(const llvm::Function* f) {
    auto fs = statistics.find(f);
    if (fs == statistics.end()) {
        statistics.emplace(f, TestStatistics::FunctionStatistics {0, 0, 1});
    } else {
        fs->second.stubs++;
    }
}

TestStatistics::FunctionStatistics TestStatistics::getStatistics(const llvm::Function* f) const {
    auto fs = statistics.find(f);
    if (fs == statistics.end()) {
        return TestStatistics::FunctionStatistics {0, 0, 0};
    } else {
        return fs->second;
    }
}
    
int TestStatistics::getTotalGood() const {
    int result = 0;
    for (auto t: statistics) {
        result += t.second.good;
    }
    return result;
}

int TestStatistics::getTotalUnknown() const {
    int result = 0;
    for (auto t: statistics) {
        result += t.second.unknown;
    }
    return result;
}

int TestStatistics::getTotalStubs() const {
    int result = 0;
    for (auto t: statistics) {
        result += t.second.stubs;
    }
    return result;
}

void TestStatistics::cleanFakeFunctions(const FunctionInfoPass& fip) {
    auto it = statistics.begin();
    while (it != statistics.end()) {
        auto fi = fip.getFunctionInfo(it->first);
        if (fi.isFake()) {
            it = statistics.erase(it);
        } else {
            it++;
        }
    }
}

char TestStatistics::ID;
static RegisterPass<TestStatistics>
X("test-statistics", "Pass that dumps test statistics");

} /* namespace borealis */
