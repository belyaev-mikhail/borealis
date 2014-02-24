/*
 * FunctionManager.h
 *
 *  Created on: Nov 15, 2012
 *      Author: ice-phoenix
 */

#ifndef FUNCTIONMANAGER_H_
#define FUNCTIONMANAGER_H_

#include <llvm/Pass.h>

#include <unordered_map>

#include "Factory/Nest.h"
#include "Logging/logger.hpp"
#include "Passes/Defect/DefectManager/DefectInfo.h"
#include "TestGen/TestSuite.h"

namespace borealis {

class FunctionManager :
        public llvm::ModulePass,
        public borealis::logging::ClassLevelLogging<FunctionManager> {

    struct FunctionDesc {
        PredicateState::Ptr Req;
        PredicateState::Ptr Bdy;
        PredicateState::Ptr Ens;
        TestSuite::Ptr Tests;

        FunctionDesc() {}

        FunctionDesc(PredicateState::Ptr Req, PredicateState::Ptr Bdy,
                     PredicateState::Ptr Ens, TestSuite::Ptr Tests) :
            Req(Req), Bdy(Bdy), Ens(Ens), Tests(Tests) {}

        FunctionDesc(PredicateState::Ptr state, TestSuite::Ptr tests)
                        : Tests(tests) {
            auto reqRest = state->splitByTypes({PredicateType::REQUIRES});
            Req = reqRest.first;

            auto ensRest = reqRest.second->splitByTypes({PredicateType::ENSURES});
            Ens = ensRest.first;
            Bdy = ensRest.second;
        }
    };

public:

    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("fm")
#include "Util/unmacros.h"

    typedef std::unordered_map<const llvm::Function*, FunctionDesc> FunctionData;
    typedef std::unordered_map<const llvm::Function*, unsigned int> Ids;
    typedef std::pair<PredicateState::Ptr, DefectInfo> Bond;
    typedef std::unordered_multimap<const llvm::Function*, Bond> FunctionBonds;

    FunctionManager();
    virtual bool runOnModule(llvm::Module&) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
    virtual ~FunctionManager() {};

    void put(const llvm::Function* F, PredicateState::Ptr state, TestSuite::Ptr tests);
    void put(const llvm::Function* F, PredicateState::Ptr state) {
        this->put(F, state, TestSuite::Ptr(new TestSuite(F)));
    }
    void put(const llvm::Function* F, TestSuite::Ptr tests) {
        this->put(F, FN.State->Basic(), tests);
    }
    void update(const llvm::Function* F, PredicateState::Ptr state, TestSuite::Ptr tests);
    void update(const llvm::Function* F, PredicateState::Ptr state) {
        this->update(F, state, TestSuite::Ptr(new TestSuite(F)));
    }
    void updateTests(const llvm::Function *F, TestSuite::Ptr tests){
        this->update(F, FN.State->Basic(), tests);
    }

    PredicateState::Ptr getReq(const llvm::Function* F) const;
    PredicateState::Ptr getBdy(const llvm::Function* F) const;
    PredicateState::Ptr getEns(const llvm::Function* F) const;
    TestSuite::Ptr getTests(const llvm::Function* F) const;

    PredicateState::Ptr getReq(const llvm::CallInst& CI, FactoryNest FN) const;
    PredicateState::Ptr getBdy(const llvm::CallInst& CI, FactoryNest FN) const;
    PredicateState::Ptr getEns(const llvm::CallInst& CI, FactoryNest FN) const;
    TestSuite::Ptr getTests(const llvm::CallInst& CI, FactoryNest FN) const;

    unsigned int getId(const llvm::Function* F) const;
    unsigned int getMemoryStart(const llvm::Function* F) const;

    void addBond(const llvm::Function* F, const std::pair<PredicateState::Ptr, DefectInfo>& bond);
    util::CollectionView<FunctionBonds::const_iterator> getBonds(const llvm::Function* F) const;

private:

    mutable FunctionData data;
    Ids ids;
    FunctionBonds bonds;

    FactoryNest FN;

    FunctionDesc get(const llvm::Function* F) const;
    FunctionDesc get(const llvm::CallInst& CI, FactoryNest FN) const;

    FunctionDesc mergeFunctionDesc(const FunctionDesc& d1, const FunctionDesc& d2) const;

};

} /* namespace borealis */

#endif /* FUNCTIONMANAGER_H_ */
