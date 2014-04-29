/*
 * CastPredicate.cpp
 *
 *  Created on: Apr 21, 2014
 *      Author: sam
 */

#include "Predicate/CastPredicate.h"
#include "Type/TypeUtils.h"

namespace borealis {

CastPredicate::CastPredicate(
        Term::Ptr lhv,
        Term::Ptr rhv,
        const Locus& loc) :
            Predicate(class_tag(*this), PredicateType::STATE, loc),
            lhv(lhv),
            rhv(rhv) {
    asString = lhv->getName() + "=" + "(" + TypeUtils::toString(*lhv->getType())
                                    + ")" + rhv->getName();
}

bool CastPredicate::equals(const Predicate* other) const {
    if (const Self* o = llvm::dyn_cast_or_null<Self>(other)) {
        return Predicate::equals(other) &&
                *lhv == *o->lhv &&
                *rhv == *o->rhv;
    } else return false;
}

size_t CastPredicate::hashCode() const {
    return util::hash::defaultHasher()(Predicate::hashCode(), lhv, rhv);
}

} /* namespace borealis */
