/*
 * DefaultSwitchCasePredicate.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: ice-phoenix
 */

#include <algorithm>

#include "Predicate/DefaultSwitchCasePredicate.h"

namespace borealis {

DefaultSwitchCasePredicate::DefaultSwitchCasePredicate(
        Term::Ptr cond,
        const std::vector<Term::Ptr>& cases,
        const Locus& loc,
        PredicateType type) :
            Predicate(class_tag(*this), type, loc) {
    ops.insert(ops.end(), cond);
    ops.insert(ops.end(), cases.begin(), cases.end());

    update();
}

Predicate* DefaultSwitchCasePredicate::update() {
    asString = getCond()->getName() + "=not(" + getCases()
                        .map([](auto&& c) { return c->getName(); })
                        .reduce("", [](auto&& acc, auto&& e) { return acc + "|" + e; }); + ")";
    return this;
}

Term::Ptr DefaultSwitchCasePredicate::getCond() const {
    return ops[0];
}

auto DefaultSwitchCasePredicate::getCases() const -> decltype(util::viewContainer(ops)) {
    return util::viewContainer(ops).drop(1);
}

} /* namespace borealis */
