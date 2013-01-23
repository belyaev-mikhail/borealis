/*
 * PredicateState.h
 *
 *  Created on: Oct 3, 2012
 *      Author: ice-phoenix
 */

#ifndef PREDICATESTATE_H_
#define PREDICATESTATE_H_

#include <llvm/Support/raw_ostream.h>
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
    PredicateState(const PredicateState& state);
    PredicateState(PredicateState&& state);

    PredicateState& operator=(const PredicateState& state);
    PredicateState& operator=(PredicateState&& state);

    PredicateState addPredicate(Predicate::Ptr pred) const;
    PredicateState addAll(const PredicateState& state) const;

    PredicateState addVisited(const llvm::Instruction* location) const;
    bool hasVisited(std::initializer_list<const llvm::Instruction*> locations) const;

    bool isUnreachable() const;

    std::pair<logic::Bool, logic::Bool> toZ3(Z3ExprFactory& z3ef) const;

    typedef std::list<Predicate::Ptr> Data;
    typedef Data::value_type DataEntry;
    typedef Data::const_iterator DataIterator;

    typedef std::unordered_set<const llvm::Instruction*> Locations;

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
    bool empty() const { return data.empty(); }

    PredicateState map(std::function<Predicate::Ptr(Predicate::Ptr)> f) {
        PredicateState res;
        for (auto& p : data) {
            res = res.addPredicate(f(p));
        }
        return res;
    }

    bool operator==(const PredicateState& other) const {
        if (this == &other) return true;

        // Workaround for std::shared_ptr::operator==
        DataIterator __end1 = data.end();
        DataIterator __end2 = other.data.end();

        DataIterator __i1 = data.begin();
        DataIterator __i2 = other.data.begin();
        while (__i1 != __end1 && __i2 != __end2 && **__i1 == **__i2) {
            ++__i1;
            ++__i2;
        }
        return __i1 == __end1 && __i2 == __end2;
    }

private:

    Data data;

    Locations visited;

};

llvm::raw_ostream& operator<<(llvm::raw_ostream& s, const PredicateState& state);
std::ostream& operator<<(std::ostream& s, const PredicateState& state);

} /* namespace borealis */

#endif /* PREDICATESTATE_H_ */
