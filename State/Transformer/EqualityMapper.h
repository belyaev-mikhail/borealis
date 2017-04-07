/*
 * EqualityMapping.h
 *
 *  Created on: 25 мая 2015 г.
 *      Author: kivi
 */

#ifndef STATE_TRANSFORMER_EQUALITYMAPPER_H_
#define STATE_TRANSFORMER_EQUALITYMAPPER_H_

#include <unordered_map>

#include "Transformer.hpp"

namespace borealis {

class EqualityMapper: public borealis::Transformer<EqualityMapper> {

    using Base = borealis::Transformer<EqualityMapper>;

public:

    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;
    using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;

    EqualityMapper(FactoryNest FN);

    Predicate::Ptr transformEqualityPredicate(EqualityPredicatePtr pred);
    Predicate::Ptr transformPredicate(Predicate::Ptr pred);

    PredicateState::Ptr transformImply(PredicateStateImplyPtr ps);

    const TermMap& getMappedValues() const;

private:

    Term::Ptr prev;
    Term::Ptr prevRhv;
    TermMap mapping;
    TermSet usedTerms;
    bool isOpaqueTerm(Term::Ptr term);

};

}  /* namespace borealis */

#endif /* STATE_TRANSFORMER_EQUALITYMAPPER_H_ */