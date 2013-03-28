/*
 * Predicate.h
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#ifndef PREDICATE_H_
#define PREDICATE_H_

#include <llvm/Instruction.h>

#include <memory>

#include "Annotation/Annotation.h"
#include "Logging/logger.hpp"
#include "Solver/ExecutionContext.h"
#include "Solver/Z3ExprFactory.h"
#include "Term/Term.h"
#include "Util/typeindex.hpp"
#include "Util/util.h"

namespace borealis {

enum class PredicateType {
    PATH,
    STATE,
    REQUIRES,
    ENSURES
};

PredicateType predicateType(const Annotation* a);

// Forward declaration
template<class SubClass>
class Transformer;

class Predicate {

public:

    typedef std::shared_ptr<const Predicate> Ptr;

    Predicate(borealis::id_t predicate_type_id);
    Predicate(borealis::id_t predicate_type_id, PredicateType type);
    virtual ~Predicate() = 0;

    borealis::id_t getPredicateTypeId() const {
        return predicate_type_id;
    }

    PredicateType getType() const {
        return type;
    }

    Predicate* setType(PredicateType type) {
        this->type = type;
        return this;
    }

    std::string toString() const {
        switch (type) {
        case PredicateType::REQUIRES: return "@R " + asString;
        case PredicateType::ENSURES: return "@E " + asString;
        case PredicateType::PATH: return "@P " + asString;
        default: return asString;
        }
    }

    virtual logic::Bool toZ3(Z3ExprFactory&, ExecutionContext* = nullptr) const = 0;

    static bool classof(const Predicate* /* t */) {
        return true;
    }

    virtual bool equals(const Predicate* other) const = 0;
    bool operator==(const Predicate& other) const {
        return this->equals(&other);
    }

    virtual size_t hashCode() const = 0;

    const llvm::Instruction* getLocation() const {
        return location;
    }

    Predicate* setLocation(const llvm::Instruction* location) {
        this->location = location;
        return this;
    }

protected:

    const borealis::id_t predicate_type_id;

    PredicateType type;
    const llvm::Instruction* location;

    // Must be set in subclasses
    std::string asString;

};

std::ostream& operator<<(std::ostream& s, const borealis::Predicate& p);

} /* namespace borealis */

namespace std {
template<>
struct hash<borealis::PredicateType> : public borealis::util::enums::enum_hash<borealis::PredicateType> {};
template<>
struct hash<const borealis::PredicateType> : public borealis::util::enums::enum_hash<borealis::PredicateType> {};
}

#endif /* PREDICATE_H_ */
