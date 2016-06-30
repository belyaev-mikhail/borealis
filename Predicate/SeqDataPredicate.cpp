/*
 * SeqDataPredicate.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#include "Predicate/SeqDataPredicate.h"

namespace borealis {

SeqDataPredicate::SeqDataPredicate(
        Term::Ptr base,
        const std::vector<Term::Ptr>& data,
        const Locus& loc,
        PredicateType type) :
            Predicate(class_tag(*this), type, loc) {
    ops.insert(ops.end(), base);
    ops.insert(ops.end(), data.begin(), data.end());

    update();
}

Predicate* SeqDataPredicate::update() {
    asString = getBase()->getName() + "=(" + getData()
                        .map([](auto&& d) { return d->getName(); })
                        .reduce("", [](auto&& acc, auto&& e) { return acc + "," + e; }) + ")";
    return this;
}

Term::Ptr SeqDataPredicate::getBase() const {
    return ops[0];
}

auto SeqDataPredicate::getData() const -> decltype(util::viewContainer(ops)) {
    return util::viewContainer(ops).drop(1);
}

} /* namespace borealis */
