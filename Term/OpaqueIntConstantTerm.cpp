/*
 * OpaqueConstantTerm.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: belyaev
 */

#include "Term/OpaqueIntConstantTerm.h"

namespace borealis {

OpaqueIntConstantTerm::OpaqueIntConstantTerm(Type::Ptr type, int64_t value):
    Term(
        class_tag(*this),
        type,
        util::toString(value)
    ), value(value) {};

Term* OpaqueIntConstantTerm::update() {
    name = util::toString(getValue());
    return this;
}

int64_t OpaqueIntConstantTerm::getValue() const {
    return value;
}

} // namespace borealis
