/*
 * ContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_
#define PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_

#include <llvm/Pass.h>
#include <llvm/IR/Instructions.h>

#include "Passes/Util/ProxyFunctionPass.h"
#include "Util/passes.hpp"
#include "Passes/Manager/ContractManager.h"
#include "Passes/PredicateStateAnalysis/PredicateStateAnalysis.h"

namespace borealis {

class ContractExtractorPass : public ProxyFunctionPass, public ShouldBeModularized {

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
    ContractManager* CM;
    PredicateStateAnalysis* PSA;

};

} /* namespace borealis */

#endif /* PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_ */
