/*
 * CallPredicate.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#include "Predicate/CallPredicate.h"

namespace borealis {

CallPredicate::CallPredicate(
        Term::Ptr funName,
        Term::Ptr result,
        const std::vector<Term::Ptr>& args,
        const Locus& loc,
        PredicateType type) :
            Predicate(class_tag(*this), type, loc) {
    ops.insert(ops.end(), funName);
    ops.insert(ops.end(), result);
    ops.insert(ops.end(), args.begin(), args.end());

    update();
}

Predicate* CallPredicate::update() {
    asString = "";
    if(getResult() != nullptr)
        asString = getResult()->getName() + "=";
    asString += getFunctionName()->getName() + "(" + getArgs()
            .map([](auto&& d) { return d->getName(); })
            .reduce("", [](auto&& acc, auto&& e) { return acc + "," + e; }) + ")";
    return this;
}

Term::Ptr CallPredicate::getFunctionName() const {
    return ops[0];
}

Term::Ptr CallPredicate::getResult() const {
    return ops[1];
}

auto CallPredicate::getArgs() const -> decltype(util::viewContainer(ops)) {
    return util::viewContainer(ops).drop(2);
}

} /* namespace borealis */
