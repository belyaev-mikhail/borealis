/*
 * BoundTerm.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: belyaev
 */

#include "Term/BoundTerm.h"

namespace borealis {

BoundTerm::BoundTerm(Type::Ptr type, Term::Ptr rhv):
    Term(
        class_tag(*this),
        type,
        "bound(" + rhv->getName() + ")"
    ) {
    subterms = { rhv };
};

Term* BoundTerm::update() {
    name = "bound(" + getRhv()->getName() + ")";
    return this;
}

Term::Ptr BoundTerm::getRhv() const {
    return subterms[0];
}

} // namespace borealis
