/*
 * CallContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORM_H_
#define STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORM_H_

#include "State/Transformer/Transformer.hpp"
#include "llvm/IR/Function.h"

namespace borealis {

class ContractExtractorTransform: public borealis::Transformer<ContractExtractorTransform> {

	typedef borealis::Transformer<ContractExtractorTransform> Base;

public:

	ContractExtractorTransform(llvm::Function* f) :
		Base(FactoryNest()),
		func(f) {}

	Predicate::Ptr trasformBase(Predicate::Ptr p) {
		return p;
	}

private:

	llvm::Function* func;

};

}  /* namespace borealis */

#endif /* STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORM_H_ */
