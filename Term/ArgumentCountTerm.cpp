/*
 * ArgumentCountTerm.cpp
 *
 *  Created on: Jan 21, 2013
 *      Author: belyaev
 */

#include "Term/ArgumentCountTerm.h"

namespace borealis {

ArgumentCountTerm::ArgumentCountTerm(Type::Ptr type):
    Term(
        class_tag(*this),
        type,
        "<arg_count>"
    ) {};

Term* ArgumentCountTerm::update() {
    return this;
}

} // namespace borealis
