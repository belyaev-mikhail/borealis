/*
 * AggregateTransformer.h
 *
 *  Created on: Jul 12, 2013
 *      Author: ice-phoenix
 */

#ifndef AGGREGATETRANSFORMER_H_
#define AGGREGATETRANSFORMER_H_

#include "State/Transformer/Transformer.hpp"
#include "Util/macros.h"

namespace borealis {

template<class First, class Rest, class = GUARD(
    std::is_base_of<Transformer<First>, First>::value &&
    std::is_base_of<Transformer<Rest>, Rest>::value
)>
class AggregateTransformer : public borealis::Transformer<AggregateTransformer<First, Rest>> {

public:

    AggregateTransformer(First&& first, Rest&& rest) :
        first(std::move(first)), rest(std::move(rest)) {}

    Predicate::Ptr transformBase(Predicate::Ptr p) {
        auto pp = first.transform(p);
        return rest.transform(pp);
    }

    Term::Ptr transformBase(Term::Ptr t) {
        auto tt = first.transform(t);
        return rest.transform(tt);
    }

private:

    First first;
    Rest rest;

};

template<class First, class Rest>
AggregateTransformer<First, Rest> operator+(First&& first, Rest&& rest) {
    return AggregateTransformer<First, Rest>(std::move(first), std::move(rest));
}

} /* namespace borealis */

#include "Util/unmacros.h"

#endif /* AGGREGATETRANSFORMER_H_ */