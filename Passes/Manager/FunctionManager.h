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

#include "Predicate/PredicateFactory.h"
#include "State/PredicateStateFactory.h"
#include "Term/TermFactory.h"

namespace borealis {

class FunctionManager : public llvm::ModulePass {

    struct FunctionDesc {
        PredicateState::Ptr Req;
        PredicateState::Ptr Bdy;
        PredicateState::Ptr Ens;

        FunctionDesc(PredicateState::Ptr Req, PredicateState::Ptr Bdy, PredicateState::Ptr Ens) :
            Req(Req), Bdy(Bdy), Ens(Ens) {}

        FunctionDesc(PredicateState::Ptr state) {
            auto reqRest = state->splitByTypes({PredicateType::REQUIRES});
            Req = reqRest.first;

            auto ensRest = reqRest.second->splitByTypes({PredicateType::ENSURES});
            Ens = ensRest.first;
            Bdy = ensRest.second;
        }
    };

public:

    static char ID;

    typedef std::unordered_map<llvm::Function*, FunctionDesc> Data;
    typedef Data::value_type DataEntry;

    FunctionManager();
    virtual bool runOnModule(llvm::Module&) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
    virtual ~FunctionManager() {};

    void put(llvm::Function* F, PredicateState::Ptr state);
    void update(llvm::Function* F, PredicateState::Ptr state);

    PredicateState::Ptr getReq(llvm::Function* F);
    PredicateState::Ptr getBdy(llvm::Function* F);
    PredicateState::Ptr getEns(llvm::Function* F);

    PredicateState::Ptr getReq(llvm::CallInst& CI, PredicateFactory* PF, TermFactory* TF);
    PredicateState::Ptr getBdy(llvm::CallInst& CI, PredicateFactory* PF, TermFactory* TF);
    PredicateState::Ptr getEns(llvm::CallInst& CI, PredicateFactory* PF, TermFactory* TF);

private:

    Data data;

    PredicateStateFactory::Ptr PSF;

    FunctionDesc get(llvm::Function* F);
    FunctionDesc get(llvm::CallInst& CI, PredicateFactory* PF, TermFactory* TF);

    FunctionDesc mergeFunctionDesc(const FunctionDesc& d1, const FunctionDesc& d2) const;

};

} /* namespace borealis */

#endif /* FUNCTIONMANAGER_H_ */
