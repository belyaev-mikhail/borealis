/*
 * PredicateStateImply.cpp
 *
 *  Created on: Apr 30, 2013
 *      Author: ice-phoenix
 */

#include "State/PredicateStateImply.h"

#include "Util/macros.h"

namespace borealis {

PredicateStateImply::PredicateStateImply(PredicateState::Ptr base, PredicateState::Ptr curr) :
        PredicateState(class_tag<Self>()),
        base(base),
        curr(curr) {
    ASSERTC(base != nullptr);
    ASSERTC(curr != nullptr);
}

PredicateState::Ptr PredicateStateImply::getBase() const {
    return base;
}

PredicateState::Ptr PredicateStateImply::getCurr() const {
    return curr;
}

PredicateState::Ptr PredicateStateImply::swapBase(PredicateState::Ptr newBase) const {
    return Simplified<Self>(
        Simplified<Self>(
            newBase,
            this->base
        ),
        this->curr
    );
}

PredicateState::Ptr PredicateStateImply::swapCurr(PredicateState::Ptr newCurr) const {
    return Simplified<Self>(
        this->base,
        Simplified<Self>(
            this->curr,
            newCurr
        )
    );
}

PredicateState::Ptr PredicateStateImply::addPredicate(Predicate::Ptr pred) const {
    ASSERTC(pred != nullptr);

    return Simplified<Self>(
        this->base,
        this->curr + pred
    );
}

PredicateState::Ptr PredicateStateImply::addVisited(const Locus& locus) const {
    return Simplified<Self>(
        this->base,
        this->curr << locus
    );
}

bool PredicateStateImply::hasVisited(std::initializer_list<Locus> loci) const {
    auto&& visited = std::unordered_set<Locus>(loci.begin(), loci.end());
    return hasVisitedFrom(visited);
}

bool PredicateStateImply::hasVisitedFrom(Loci& visited) const {
    return curr->hasVisitedFrom(visited) || base->hasVisitedFrom(visited);
}

PredicateState::Loci PredicateStateImply::getVisited() const {
    Loci res;
    auto&& baseLoci = base->getVisited();
    res.insert(baseLoci.begin(), baseLoci.end());
    auto&& currLoci = curr->getVisited();
    res.insert(currLoci.begin(), currLoci.end());
    return res;
}

PredicateStateImply::SelfPtr PredicateStateImply::fmap_(FMapper f) const {
    return Uniquified(
        f(base),
        f(curr)
    );
}

PredicateState::Ptr PredicateStateImply::fmap(FMapper f) const {
    return Simplified(fmap_(f).release());
}

PredicateState::Ptr PredicateStateImply::reverse() const {
    return Simplified<Self>(
        curr->reverse(),
        base->reverse()
    );
}

std::pair<PredicateState::Ptr, PredicateState::Ptr> PredicateStateImply::splitByTypes(
        std::initializer_list<PredicateType> types) const {
    auto&& baseSplit = base->splitByTypes(types);
    auto&& currSplit = curr->splitByTypes(types);

    return std::make_pair(
        Simplified<Self>(baseSplit.first, currSplit.first),
        Simplified<Self>(baseSplit.second, currSplit.second)
    );
}

PredicateState::Ptr PredicateStateImply::sliceOn(PredicateState::Ptr on) const {
    if (*base == *on) {
        return curr;
    } else if (auto&& baseSlice = base->sliceOn(on)) {
        return Simplified<Self>(baseSlice, curr);
    }
    return nullptr;
}

PredicateState::Ptr PredicateStateImply::simplify() const {
    auto&& res = fmap_([](auto&& s) { return s->simplify(); });

    if (res->curr->isEmpty()) {
        return res->base;
    }
    if (res->base->isEmpty()) {
        return res->curr;
    }

    return PredicateState::Ptr(res.release());
}

bool PredicateStateImply::isEmpty() const {
    return curr->isEmpty() && base->isEmpty();
}

unsigned int PredicateStateImply::size() const {
    return curr->size() + base->size();
}

bool PredicateStateImply::equals(const PredicateState* other) const {
    if (auto* o = llvm::dyn_cast_or_null<Self>(other)) {
        return PredicateState::equals(other) &&
                *this->base == *o->base &&
                *this->curr == *o->curr;
    } else return false;
}

borealis::logging::logstream& PredicateStateImply::dump(borealis::logging::logstream& s) const {
    return s << base << "=>" << curr;
}

std::string PredicateStateImply::toString() const {
    return base->toString() + "=>" + curr->toString();
}

} /* namespace borealis */

#include "Util/unmacros.h"
