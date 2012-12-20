/*
 * PredicateState.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: ice-phoenix
 */

#include "PredicateState.h"

#include "Logging/tracer.hpp"
#include "Solver/ExecutionContext.h"

namespace borealis {

using util::contains;
using util::view;

PredicateState::PredicateState() {
}

PredicateState::PredicateState(const PredicateState& state) :
            data(state.data), visited(state.visited) {
}

PredicateState::PredicateState(PredicateState&& state) :
            data(std::move(state.data)), visited(std::move(state.visited)) {
}

PredicateState& PredicateState::operator=(const PredicateState& state) {
    data = state.data;
    visited = state.visited;
    return *this;
}

PredicateState& PredicateState::operator=(PredicateState&& state) {
    data = std::move(state.data);
    visited = std::move(state.visited);
    return *this;
}

PredicateState PredicateState::addPredicate(Predicate::Ptr pred) const {
    PredicateState res = PredicateState(*this);
    res.data.push_back(pred);
    res.visited.insert(pred->getLocation());
    return res;
}

PredicateState PredicateState::addAll(const PredicateState& state) const {
    PredicateState res = PredicateState(*this);
    for (auto& p : state) {
        res.data.push_back(p);
        res.visited.insert(p->getLocation());
    }
    return res;
}

PredicateState PredicateState::addVisited(const llvm::Instruction* location) const {
    PredicateState res = PredicateState(*this);
    res.visited.insert(location);
    return res;
}

std::pair<logic::Bool, logic::Bool> PredicateState::toZ3(Z3ExprFactory& z3ef) const {
    using namespace::z3;

    TRACE_FUNC;

    ExecutionContext ctx(z3ef);

    auto path = std::vector<logic::Bool>();
    auto state = std::vector<logic::Bool>();

    for (auto& v : data) {
        if (v->getType() == PredicateType::PATH) {
            path.push_back(v->toZ3(z3ef, &ctx));
        } else if (v->getType() == PredicateType::STATE) {
            state.push_back(v->toZ3(z3ef, &ctx));
        }
    }

    auto p = z3ef.getTrue();
    for (const auto& e : path) {
        p = p && e;
    }

    auto s = z3ef.getTrue();
    for (const auto& e : state) {
        s = s && e;
    }
    s = s && ctx.toZ3();

    return std::make_pair(p, s);
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& s, const PredicateState& state) {
    using borealis::util::streams::endl;
    s << '(';
    if (!state.empty()) {
        auto iter = state.begin();
        const auto& el = *iter++;
        s << endl << "  " << el->toString();
        for (const auto& e : view(iter, state.end())) {
            s << ',' << endl << "  " << e->toString();
        }
    }
    s << endl << ')';
    return s;
}

std::ostream& operator<<(std::ostream& s, const PredicateState& state) {
    using std::endl;
    s << '(';
    if (!state.empty()) {
        auto iter = state.begin();
        const auto& el = *iter++;
        s << endl << "  " << el->toString();
        for (const auto& e : view(iter, state.end())) {
            s << ',' << endl << "  " << e->toString();
        }
    }
    s << endl << ')';
    return s;
}

} /* namespace borealis */
