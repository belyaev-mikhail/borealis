/*
 * MallocPredicate.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: ice-phoenix
 */

#include "Predicate/MallocPredicate.h"

namespace borealis {

MallocPredicate::MallocPredicate(
        Term::Ptr lhv,
        Term::Ptr numElems,
        Term::Ptr origNumElems,
        const Locus& loc,
        PredicateType type) :
            Predicate(class_tag(*this), type, loc) {
    ops = { lhv, numElems, origNumElems };
    update();
}

Predicate* MallocPredicate::update() {
    asString = getLhv()->getName() + "=malloc(" +
        getNumElems()->getName() + "," +
        getOrigNumElems()->getName() +
    ")";
    return this;
}

Term::Ptr MallocPredicate::getLhv() const {
    return ops[0];
}

Term::Ptr MallocPredicate::getNumElems() const {
    return ops[1];
}

Term::Ptr MallocPredicate::getOrigNumElems() const {
    return ops[2];
}

} /* namespace borealis */
