/*
 * Predicate.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#include "Predicate.h"

namespace borealis {

Predicate::Predicate(borealis::id_t predicate_type_id) :
        Predicate(predicate_type_id, PredicateType::STATE) {}

Predicate::Predicate(borealis::id_t predicate_type_id, PredicateType type) :
        predicate_type_id(predicate_type_id),
        type(type),
        location(nullptr) {}

Predicate::~Predicate() {}

llvm::raw_ostream& operator<<(llvm::raw_ostream& s, const borealis::Predicate& p) {
    s << p.toString();
    return s;
}

std::ostream& operator<<(std::ostream& s, const borealis::Predicate& p) {
    s << p.toString();
    return s;
}

} // namespace borealis
