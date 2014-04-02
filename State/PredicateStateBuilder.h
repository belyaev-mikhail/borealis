/*
 * PredicateStateBuilder.h
 *
 *  Created on: May 23, 2013
 *      Author: ice-phoenix
 */

#ifndef PREDICATESTATEBUILDER_H_
#define PREDICATESTATEBUILDER_H_

#include "State/PredicateStateFactory.h"

namespace borealis {

class PredicateStateBuilder {

private:

    PredicateStateFactory::Ptr PSF;
    PredicateState::Ptr State;

public:

    PredicateStateBuilder(PredicateStateFactory::Ptr PSF, PredicateState::Ptr state);
    PredicateStateBuilder(PredicateStateFactory::Ptr PSF, Predicate::Ptr pred);
    PredicateStateBuilder(const PredicateStateBuilder&) = default;
    PredicateStateBuilder(PredicateStateBuilder&&) = default;

    PredicateState::Ptr operator()() const;

    PredicateStateBuilder& operator+=(PredicateState::Ptr s);
    PredicateStateBuilder& operator+=(Predicate::Ptr p);

    friend PredicateStateBuilder operator+ (PredicateStateBuilder PSB, PredicateState::Ptr s);
    friend PredicateStateBuilder operator+ (PredicateStateBuilder PSB, Predicate::Ptr p);
    friend PredicateStateBuilder operator<<(PredicateStateBuilder PSB, const Locus& loc);

};

PredicateStateBuilder operator*(PredicateStateFactory::Ptr PSF, PredicateState::Ptr state);
PredicateStateBuilder operator*(PredicateStateFactory::Ptr PSF, Predicate::Ptr p);

template<class Container>
PredicateStateBuilder operator*(PredicateStateFactory::Ptr PSF, Container&& c) {
    PredicateStateBuilder res{PSF, PSF->Basic()};
    for (const auto& p : c) {
        res += p;
    }
    return res;
}

PredicateStateBuilder operator+ (PredicateStateBuilder PSB, PredicateState::Ptr s);
PredicateStateBuilder operator+ (PredicateStateBuilder PSB, Predicate::Ptr p);
PredicateStateBuilder operator<<(PredicateStateBuilder PSB, const Locus& loc);

template<class Container>
PredicateStateBuilder operator+(PredicateStateBuilder PSB, Container&& c) {
    PredicateStateBuilder res{PSB};
    for (const auto& p : c) {
        res += p;
    }
    return res;
}

} /* namespace borealis */

#endif /* PREDICATESTATEBUILDER_H_ */
