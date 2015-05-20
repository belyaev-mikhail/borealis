/*
 * CallContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORM_H_
#define STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORM_H_

#include "Factory/Nest.h"
#include "State/Transformer/Transformer.hpp"
#include "llvm/IR/Function.h"
#include <vector>

namespace borealis {

class ContractExtractorTransform: public borealis::Transformer<ContractExtractorTransform> {

	using Base = borealis::Transformer<ContractExtractorTransform>;
	using Args = std::vector<Term::Ptr>;

public:

	ContractExtractorTransform(FactoryNest fn, llvm::Function* f) :
		Base(FactoryNest()),
		FN(fn) {
		for(auto&& it : f->args()) {
			args.push_back(FN.Term->getArgumentTerm(&it));
		}
	}

	Predicate::Ptr trasformBase(Predicate::Ptr p) {
		return p;
	}

	Predicate::Ptr transformPredicate(Predicate::Ptr p) {
		return p;
	}

private:

	Args args;
	FactoryNest FN;


};

}  /* namespace borealis */

#endif /* STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORM_H_ */
