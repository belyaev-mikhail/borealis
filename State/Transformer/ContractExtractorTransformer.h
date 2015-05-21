/*
 * CallContractExtractor.h
 *
 *  Created on: 19 мая 2015 г.
 *      Author: kivi
 */

#ifndef STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORMER_H_
#define STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORMER_H_

#include "State/Transformer/Transformer.hpp"
#include <unordered_set>

namespace borealis {

class ContractExtractorTransformer: public borealis::Transformer<ContractExtractorTransformer> {

	using Base = borealis::Transformer<ContractExtractorTransformer>;
	using Args = std::unordered_set<Term::Ptr, TermHash, TermEquals>;

public:

	ContractExtractorTransformer(const FactoryNest& fn, llvm::CallInst& I);

	PredicateState::Ptr transform(PredicateState::Ptr ps);
	Predicate::Ptr transformPredicate(Predicate::Ptr pred);


private:

	Args args;
	FactoryNest FN;

};

}  /* namespace borealis */

#endif /* STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORMER_H_ */
