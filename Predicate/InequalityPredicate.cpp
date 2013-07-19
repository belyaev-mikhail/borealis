/*
 * InequalityPredicate.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: ice-phoenix
 */

#include "Predicate/InequalityPredicate.h"

namespace borealis {

InequalityPredicate::InequalityPredicate(
        Term::Ptr lhv,
        Term::Ptr rhv,
        PredicateType type) :
            Predicate(type_id(*this), type),
            lhv(lhv),
            rhv(rhv) {
    this->asString = this->lhv->getName() + "!=" + this->rhv->getName();
}

bool InequalityPredicate::equals(const Predicate* other) const {
    if (const Self* o = llvm::dyn_cast_or_null<Self>(other)) {
        return *this->lhv == *o->lhv &&
                *this->rhv == *o->rhv;
    } else return false;
}

size_t InequalityPredicate::hashCode() const {
    return util::hash::defaultHasher()(type, lhv, rhv);
}

} /* namespace borealis */
