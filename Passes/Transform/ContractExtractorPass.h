/*
 * ContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_
#define PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_

#include <set>

#include <llvm/Pass.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Analysis/AliasAnalysis.h>

#include "Passes/Util/ProxyFunctionPass.h"
#include "Util/passes.hpp"
#include "Passes/Contract/ContractManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"

namespace borealis {

class ContractExtractorPass : public ProxyFunctionPass, public ShouldBeModularized {

    using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;

private:

	void processCallInstruction(llvm::CallInst& I, PredicateState::Ptr S);

public:

    static char ID;

    ContractExtractorPass();
    virtual ~ContractExtractorPass() = default;

    virtual bool runOnFunction(llvm::Function& F) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;

private:

    FactoryNest FN;
    FunctionManager* FM;
    ContractManager* CM;
    PredicateStateAnalysis* PSA;
    llvm::AliasAnalysis AA;
};

} /* namespace borealis */

#endif /* PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_ */
