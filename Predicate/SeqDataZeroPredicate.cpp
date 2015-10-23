/*
 * SeqDataZeroPredicate.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#include "Predicate/SeqDataZeroPredicate.h"

namespace borealis {

SeqDataZeroPredicate::SeqDataZeroPredicate(
        Term::Ptr base,
        size_t size,
        const Locus& loc,
        PredicateType type) :
            Predicate(class_tag(*this), type, loc), size(size) {

    ops.insert(ops.end(), base);
    update();
}

Predicate* SeqDataZeroPredicate::update() {
    asString = getBase()->getName() + "=(0 x " + util::toString(size) + ")";
    return this;
}

Term::Ptr SeqDataZeroPredicate::getBase() const {
    return ops[0];
}

} /* namespace borealis */
