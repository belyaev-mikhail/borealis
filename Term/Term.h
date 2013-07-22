/*
 * Term.h
 *
 *  Created on: Nov 16, 2012
 *      Author: ice-phoenix
 */

#ifndef TERM_H_
#define TERM_H_

#include <string>

#include "SMT/SMTUtil.h"
#include "Type/TypeFactory.h"
#include "Util/typeindex.hpp"
#include "Util/util.h"

namespace borealis {

// Forward declarations
template<class SubClass> class Transformer;
// End of forward declarations

class Term {

public:

    typedef std::shared_ptr<const Term> Ptr;

protected:

    Term(id_t id, const std::string& name, id_t term_type_id) :
        id(id), term_type_id(term_type_id), name(name) {};
    Term(const Term&) = default;
    virtual ~Term() {};

public:

    id_t getId() const {
        return id;
    }

    const std::string& getName() const {
        return name;
    }

    id_t getTermTypeId() const {
        return term_type_id;
    }

    virtual bool equals(const Term* other) const {
        if (other == nullptr) return false;
        return this->id == other->id &&
                this->term_type_id == other->term_type_id &&
                this->name == other->name;
    }

    bool operator==(const Term& other) const {
        if (this == &other) return true;
        return this->equals(&other);
    }

    size_t hashCode() const {
        return static_cast<size_t>(id) ^
               static_cast<size_t>(term_type_id) ^
               std::hash<std::string>()(name);
    }

    static bool classof(const Term*) {
        return true;
    }

    virtual Type::Ptr getTermType() const = 0;

private:

    const id_t id;
    const id_t term_type_id;

protected:

    std::string name;

};

} /* namespace borealis */

namespace std {
template<>
struct hash<borealis::Term::Ptr> {
    size_t operator()(const borealis::Term::Ptr& t) const {
        return t->hashCode();
    }
};
template<>
struct hash<const borealis::Term::Ptr> {
    size_t operator()(const borealis::Term::Ptr& t) const {
        return t->hashCode();
    }
};
} // namespace std

#define MK_COMMON_TERM_IMPL(CLASS) \
private: \
    typedef CLASS Self; \
    CLASS(const CLASS&) = default; \
public: \
    friend class TermFactory; \
    virtual ~CLASS() {}; \
    static bool classof(const Self*) { \
        return true; \
    } \
    static bool classof(const Term* t) { \
        return t->getTermTypeId() == type_id<Self>(); \
    }

#endif /* TERM_H_ */
