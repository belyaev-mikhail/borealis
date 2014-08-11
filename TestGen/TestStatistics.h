/* 
 * File:   TestStatistics.h
 * Author: maxim
 *
 * Created on 11 Август 2014 г., 12:09
 */

#include <unordered_map>

#include <llvm/Function.h>
#include <llvm/Pass.h>

#include "Logging/logger.hpp"
#include "Passes/TestGeneration/FunctionInfoPass.h"

#ifndef TESTSTATISTICS_H
#define	TESTSTATISTICS_H

namespace borealis {

class TestStatistics :
        public llvm::ImmutablePass,
        public borealis::logging::ClassLevelLogging<TestStatistics> {
public:
    struct FunctionStatistics {
        int good;
        int unknown;
        int stubs;
    };

private:
    typedef std::unordered_map<const llvm::Function*, FunctionStatistics> statMap;
    typedef statMap::const_iterator const_iterator;

public:
    
    static char ID;
    
#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("TestStatistics")
#include "Util/unmacros.h"
    
    TestStatistics();
    virtual ~TestStatistics() {};
    
    virtual void getAnalysisUsage(llvm::AnalysisUsage & AU) const override;
    
    void addGoodTest(const llvm::Function* f);
    void addUnknownTest(const llvm::Function* f);
    void addStubTest(const llvm::Function* f);
    
    FunctionStatistics getStatistics(const llvm::Function* f) const;
    
    int getTotalGood() const;
    int getTotalUnknown() const;
    int getTotalStubs() const;
    
    const_iterator begin() const {return statistics.begin();}
    const_iterator end() const {return statistics.end();}
    
    void cleanFakeFunctions(const FunctionInfoPass& fip);
    
private:
    
    statMap statistics;
    
};

} /* namespace borealis */

#endif	/* TESTSTATISTICS_H */

