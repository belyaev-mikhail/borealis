/*
 * AllocaPredicate.cpp
 *
 *  Created on: Nov 28, 2012
 *      Author: belyaev
 */

#include "Predicate/AllocaPredicate.h"

namespace borealis {

AllocaPredicate::AllocaPredicate(
        Term::Ptr lhv,
        Term::Ptr numElements,
        Term::Ptr origNumElements,
        const Locus& loc,
        PredicateType type) :
            Predicate(class_tag(*this), type, loc) {
    ops = { lhv, numElements, origNumElements };
    update();
}

Predicate* AllocaPredicate::update() {
    asString = getLhv()->getName() + "=alloca(" +
        getNumElems()->getName() + "," +
        getOrigNumElems()->getName() +
    ")";
    return this;
}

Term::Ptr AllocaPredicate::getLhv() const {
    return ops[0];
}

Term::Ptr AllocaPredicate::getNumElems() const {
    return ops[1];
}

Term::Ptr AllocaPredicate::getOrigNumElems() const {
    return ops[2];
}

} /* namespace borealis */
