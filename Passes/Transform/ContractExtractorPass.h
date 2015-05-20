/*
 * ContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_
#define PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_

#include "llvm/Pass.h"

namespace borealis {

class ContractExtractorPass : public llvm::FunctionPass {
public:
    static char ID;

    ContractExtractorPass();
    virtual ~ContractExtractorPass() = default;

    virtual bool runOnFunction(llvm::Function& F) override;

    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;
};

} /* namespace borealis */

#endif /* PASSES_TRANSFORM_CONTRACTEXTRACTORPASS_H_ */
