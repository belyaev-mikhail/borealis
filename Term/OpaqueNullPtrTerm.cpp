/*
 * OpaqueNullPtrTerm.cpp
 *
 *  Created on: Jan 21, 2013
 *      Author: belyaev
 */

#include "Term/OpaqueNullPtrTerm.h"

namespace borealis {

OpaqueNullPtrTerm::OpaqueNullPtrTerm(Type::Ptr type):
    Term(
        class_tag(*this),
        type,
        "<nullptr>"
    ) {};

Term* OpaqueNullPtrTerm::update() {
    return this;
}

} // namespace borealis
