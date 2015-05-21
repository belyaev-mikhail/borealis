/*
 * ContractExtractorTransformer.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */
#include "State/Transformer/ContractExtractorTransformer.h"

namespace borealis {

	ContractExtractorTransformer::ContractExtractorTransformer(const FactoryNest& fn, llvm::CallInst& I) :
		Transformer(fn),
		FN(fn) {
		for(auto&& it : I.arg_operands()) {
			args.insert(FN.Term->getValueTerm(&*it));
		}
	}

	PredicateState::Ptr ContractExtractorTransformer::transform(PredicateState::Ptr ps) {
		return Base::transform(ps)
			->filter([](auto&& p) { return !!p; })
			->simplify();
	}

	Predicate::Ptr ContractExtractorTransformer::transformPredicate(Predicate::Ptr pred) {
		if(pred->getType() == PredicateType::PATH) {
			for(auto&& i : pred->getOperands()) {
				for(auto&& it : Term::getFullTermSet(i)) {
					if(util::contains(args, it)) {
						return pred;
					}
				}
			}
		}
		return nullptr;
	}

}  /* namespace borealis */



