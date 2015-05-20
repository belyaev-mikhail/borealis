/*
 * ContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_
#define PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_

#include "llvm/Pass.h"

#include "Passes/Util/ProxyFunctionPass.h"
#include "Util/passes.hpp"
#include "State/PredicateState.h"
#include "Factory/Nest.h"

namespace borealis {

class ContractExtractorPass : public ProxyFunctionPass, public ShouldBeModularized {
private:

	void processCallInstruction(llvm::CallInst& I, borealis::PredicateState::Ptr S);

public:
    static char ID;

    ContractExtractorPass();
    virtual ~ContractExtractorPass() = default;

    virtual bool runOnFunction(llvm::Function& F) override;

    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;

private:
    FactoryNest FN;
};

} /* namespace borealis */

#endif /* PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_ */
