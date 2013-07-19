/*
 * GlobalsPredicate.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: ice-phoenix
 */

#include "Predicate/GlobalsPredicate.h"

namespace borealis {

GlobalsPredicate::GlobalsPredicate(
        const std::vector<Term::Ptr>& globals,
        PredicateType type) :
            Predicate(type_id(*this), type),
            globals(globals) {

    using borealis::util::head;
    using borealis::util::tail;

    std::string a{""};

    if (!globals.empty()) {
        a = head(globals)->getName();
        for (const auto& g : tail(globals)) {
            a = a + "," + g->getName();
        }
    }

    this->asString = "globals(" + a + ")";
}

bool GlobalsPredicate::equals(const Predicate* other) const {
    if (const Self* o = llvm::dyn_cast_or_null<Self>(other)) {
        return std::equal(globals.begin(), globals.end(), o->globals.begin(),
            [](const Term::Ptr& a, const Term::Ptr& b) { return *a == *b; }
        );
    } else return false;
}

size_t GlobalsPredicate::hashCode() const {
    return util::hash::defaultHasher()(type, globals);
}

} /* namespace borealis */
