/*
 * OpaqueUndefTerm.cpp
 *
 *  Created on: Jan 21, 2013
 *      Author: belyaev
 */

#include "Term/OpaqueUndefTerm.h"

namespace borealis {

OpaqueUndefTerm::OpaqueUndefTerm(Type::Ptr type):
    Term(
        class_tag(*this),
        type,
        "<undef>"
    ) {};

Term* OpaqueUndefTerm::update() {
    return this;
}

} // namespace borealis
