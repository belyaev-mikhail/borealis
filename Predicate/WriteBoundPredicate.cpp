/*
 * WriteBoundPredicate.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#include "Predicate/WriteBoundPredicate.h"

namespace borealis {

WriteBoundPredicate::WriteBoundPredicate(
        Term::Ptr lhv,
        Term::Ptr rhv,
        const Locus& loc,
        PredicateType type) :
            Predicate(class_tag(*this), type, loc) {
    ops = { lhv, rhv };
    update();
}

Predicate* WriteBoundPredicate::update() {
    asString = "writeBound(" +
        getLhv()->getName() + "," +
        getRhv()->getName() +
    ")";
    return this;
}

Term::Ptr WriteBoundPredicate::getLhv() const {
    return ops[0];
}

Term::Ptr WriteBoundPredicate::getRhv() const {
    return ops[1];
}

} /* namespace borealis */
