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

        EqualityMapper(FactoryNest FN);

        Predicate::Ptr transformEquality(EqualityPredicatePtr pred);
        Predicate::Ptr transformPredicate(Predicate::Ptr pred);
        bool isOpaqueTerm(Term::Ptr term);

        const TermMap& getMappedValues() const;

    private:

        TermMap mapping;

    };

}  /* namespace borealis */

#endif /* STATE_TRANSFORMER_EQUALITYMAPPER_H_ */