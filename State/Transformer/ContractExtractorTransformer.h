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
#include <unordered_set>

namespace borealis {

class ContractExtractorTransformer: public borealis::Transformer<ContractExtractorTransformer> {

	using Base = borealis::Transformer<ContractExtractorTransformer>;
	using Args = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
	using Mapper = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;

public:

	ContractExtractorTransformer(const FactoryNest& fn, llvm::CallInst& I, Mapper& m);

	PredicateState::Ptr transform(PredicateState::Ptr ps);
	Predicate::Ptr transformPredicate(Predicate::Ptr pred);

private:

	bool checkTerm(Term::Ptr term);

private:

	Args args;
	FactoryNest FN;
	Mapper mapping;

};

}  /* namespace borealis */

#endif /* STATE_TRANSFORMER_CONTRACTEXTRACTORTRANSFORMER_H_ */
