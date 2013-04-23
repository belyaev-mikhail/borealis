/*
 * PredicateState.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: ice-phoenix
 */

#include "Logging/tracer.hpp"
#include "Solver/ExecutionContext.h"
#include "Solver/Z3Solver.h"
#include "State/PredicateState.h"

#include "Util/macros.h"

namespace borealis {

using borealis::util::contains;
using borealis::util::view;

PredicateState::PredicateState() {}

PredicateState::PredicateState(Predicate::Ptr p) {
    data.push_back(p);
    visited.insert(p->getLocation());
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
    ASSERT(pred != nullptr, "Trying to add an empty predicate");

    PredicateState res = PredicateState(*this);
    res.data.push_back(pred);
    res.visited.insert(pred->getLocation());
    return res;
}

PredicateState PredicateState::addAll(const PredicateState& state) const {
    if (state.isEmpty()) return *this;

    PredicateState res = PredicateState(*this);
    res.data.insert(res.data.end(), state.data.begin(), state.data.end());
    res.visited.insert(state.visited.begin(), state.visited.end());
    return res;
}

PredicateState PredicateState::addVisited(const llvm::Value* location) const {
    PredicateState res = PredicateState(*this);
    res.visited.insert(location);
    return res;
}

bool PredicateState::isUnreachable() const {
    z3::context ctx;
    Z3ExprFactory z3ef(ctx);
    Z3Solver s(z3ef);

    auto split = this->splitByTypes({PredicateType::PATH});
    return s.checkPathPredicates(split.first, split.second);
}

logic::Bool PredicateState::toZ3(Z3ExprFactory& z3ef, ExecutionContext* pctx) const {
    TRACE_FUNC;

    ExecutionContext ctx(z3ef);
    if (!pctx) pctx = &ctx;

    auto res = z3ef.getTrue();
    for (auto& v : data) {
        res = res && v->toZ3(z3ef, pctx);
    }
    res = res && pctx->toZ3();

    return res;
}

std::ostream& operator<<(std::ostream& s, const PredicateState& state) {
    using std::endl;
    s << '(';
    if (!state.isEmpty()) {
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

////////////////////////////////////////////////////////////////////////////////
//
// PredicateState operators
//
////////////////////////////////////////////////////////////////////////////////

const PredicateState operator&&(const PredicateState& state, Predicate::Ptr p) {
    return state.addPredicate(p);
}

const PredicateState operator+(const PredicateState& state, Predicate::Ptr p) {
    return state.addPredicate(p);
}

const PredicateState operator&&(const PredicateState& a, const PredicateState& b) {
    return a.addAll(b);
}

const PredicateState operator+(const PredicateState& a, const PredicateState& b) {
    return a.addAll(b);
}

const PredicateState operator<<(const PredicateState& a, const llvm::Value* loc) {
    return a.addVisited(loc);
}

const PredicateState operator<<(const PredicateState& a, const llvm::Value& loc) {
    return a.addVisited(&loc);
}

} /* namespace borealis */

#include "Util/unmacros.h"
