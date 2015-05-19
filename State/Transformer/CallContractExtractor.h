/*
 * CallContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef STATE_TRANSFORMER_CALLCONTRACTEXTRACTOR_H_
#define STATE_TRANSFORMER_CALLCONTRACTEXTRACTOR_H_

#include "State/Transformer/Transformer.hpp"
#include "llvm/IR/Function.h"

namespace borealis {

class CallContractExtractor: public borealis::Transformer<CallContractExtractor> {

	typedef borealis::Transformer<CallContractExtractor> Base;

public:

	CallContractExtractor(PredicateState::Ptr s, llvm::Function* f) :
		Base(FactoryNest()),
		state(s),
		func(f) {}

	Predicate::Ptr trasformBase(Predicate::Ptr p) {
		return p;
	}

private:

	PredicateState::Ptr state;
	llvm::Function* func;

};

}  /* namespace borealis */

#endif /* STATE_TRANSFORMER_CALLCONTRACTEXTRACTOR_H_ */
