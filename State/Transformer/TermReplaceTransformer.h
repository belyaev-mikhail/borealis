/*
 * TermReplaceTransformer.h
 *
 *  Created on: 19 июня 2015 г.
 *      Author: kivi
 */

#ifndef STATE_TRANSFORMER_TERMREPLACETRANSFORMER_H_
#define STATE_TRANSFORMER_TERMREPLACETRANSFORMER_H_

#include <unordered_map>


#include "Transformer.hpp"
#include "../../Factory/Nest.h"
#include "../../Term/Term.h"

namespace borealis {

class TermReplaceTransformer : public borealis::Transformer<TermReplaceTransformer> {

    using Base = borealis::Transformer<TermReplaceTransformer>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;


public:

    TermReplaceTransformer(const FactoryNest& fn, const TermMap& a);

    Term::Ptr transformTerm(Term::Ptr term);

private:

    TermMap termMap;

};

}  /* namespace borealis */


#endif /* STATE_TRANSFORMER_TERMREPLACETRANSFORMER_H_ */
