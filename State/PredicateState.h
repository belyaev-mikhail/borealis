/*
 * PredicateState.h
 *
 *  Created on: Oct 3, 2012
 *      Author: ice-phoenix
 */

#ifndef PREDICATESTATE_H_
#define PREDICATESTATE_H_

#include <llvm/Value.h>

#include <functional>
#include <initializer_list>
#include <list>
#include <unordered_set>

#include "Logging/logger.hpp"
#include "Predicate/Predicate.h"
#include "Solver/Z3ExprFactory.h"
#include "Util/util.h"

namespace borealis {

class PredicateState {

public:

    PredicateState();
    PredicateState(Predicate::Ptr p);
    PredicateState(const PredicateState& state) = default;
    PredicateState(PredicateState&& state) = default;

    PredicateState& operator=(const PredicateState& state);
    PredicateState& operator=(PredicateState&& state);

    PredicateState addPredicate(Predicate::Ptr pred) const;
    PredicateState addAll(const PredicateState& state) const;

    PredicateState addVisited(const llvm::Value* location) const;

    template<class H, class ...T>
    bool hasVisited(H* loc, T&... rest) const {
        if (!borealis::util::contains(visited, loc)) return false;
        return hasVisited(rest...);
    }

    template<class H, class ...T>
    bool hasVisited(H& loc, T&... rest) const {
        if (!borealis::util::contains(visited, &loc)) return false;
        return hasVisited(rest...);
    }

    template<class H>
    bool hasVisited(H* loc) const {
        return borealis::util::contains(visited, loc);
    }

    template<class H>
    bool hasVisited(H& loc) const {
        return borealis::util::contains(visited, &loc);
    }

    bool isUnreachable() const;

    logic::Bool toZ3(Z3ExprFactory& z3ef, ExecutionContext* pctx = nullptr) const;

    typedef std::list<Predicate::Ptr> Data;
    typedef Data::value_type DataEntry;
    typedef Data::const_iterator DataIterator;

    typedef std::unordered_set<const llvm::Value*> Locations;

    struct Hash {
        static size_t hash(const PredicateState& state) {
            size_t h = 17;
            for (const auto& e : state) {
                h = 3 * h + e->hashCode();
            }
            return h;
        }

        size_t operator()(const PredicateState& state) const {
            return hash(state);
        }
    };

    DataIterator begin() const { return data.begin(); }
    DataIterator end() const { return data.end(); }
    bool isEmpty() const { return data.empty(); }

    static const PredicateState& empty() {
        static PredicateState empty;
        return empty;
    }

    template<class Mapper>
    PredicateState map(Mapper f) const {
        PredicateState res;
        for (auto& p : data) res = res.addPredicate(f(p));
        return res;
    }

    PredicateState filterByTypes(std::initializer_list<PredicateType> types) const {
        PredicateState res;
        for (auto& p : data) {
            if (std::any_of(types.begin(), types.end(),
                [&p](const PredicateType& type) { return p->getType() == type; })) {
                res = res.addPredicate(p);
            }
        }
        return res;
    }

    template<class Condition>
    PredicateState filter(Condition f) const {
        PredicateState res;
        for (auto& p : data) if (f(p)) res = res.addPredicate(p);
        return res;
    }

    std::pair<PredicateState, PredicateState> splitByTypes(std::initializer_list<PredicateType> types) const {
        PredicateState yes, no;
        for (auto& p : data) {
            if (std::any_of(types.begin(), types.end(),
                [&p](const PredicateType& type) { return p->getType() == type; })) {
                yes = yes.addPredicate(p);
            } else {
                no = no.addPredicate(p);
            }
        }
        return std::make_pair(yes, no);
    }

    bool operator==(const PredicateState& other) const {
        if (this == &other) return true;

        return std::equal(begin(), end(), other.begin(),
            [](const Predicate::Ptr& a, const Predicate::Ptr& b) {
                return *a == *b;
            });
    }

private:

    Data data;

    Locations visited;

};

std::ostream& operator<<(std::ostream& s, const PredicateState& state);

////////////////////////////////////////////////////////////////////////////////
//
// PredicateState operators
//
////////////////////////////////////////////////////////////////////////////////

const PredicateState operator&&(const PredicateState& state, Predicate::Ptr p);
const PredicateState operator+(const PredicateState& state, Predicate::Ptr p);

const PredicateState operator&&(const PredicateState& a, const PredicateState& b);
const PredicateState operator+(const PredicateState& a, const PredicateState& b);

const PredicateState operator<<(const PredicateState& a, const llvm::Value* loc);
const PredicateState operator<<(const PredicateState& a, const llvm::Value& loc);

} /* namespace borealis */

#endif /* PREDICATESTATE_H_ */
