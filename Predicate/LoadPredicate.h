/*
 * LoadPredicate.h
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#ifndef LOADPREDICATE_H_
#define LOADPREDICATE_H_

#include "Predicate.h"

namespace borealis {

class PredicateFactory;

class LoadPredicate: public Predicate {

public:

    virtual Predicate::Key getKey() const;

    virtual logic::Bool toZ3(Z3ExprFactory& z3ef, ExecutionContext* = nullptr) const;

    static bool classof(const Predicate* p) {
        return p->getPredicateTypeId() == type_id<LoadPredicate>();
    }

    static bool classof(const LoadPredicate* /* p */) {
        return true;
    }

    template<class SubClass>
    const LoadPredicate* accept(Transformer<SubClass>* t) const {
        return new LoadPredicate(
                Term::Ptr(t->transform(lhv.get())),
                Term::Ptr(t->transform(rhv.get())));
    }

    virtual bool equals(const Predicate* other) const;
    virtual size_t hashCode() const;

    friend class PredicateFactory;

private:

    const Term::Ptr lhv;
    const Term::Ptr rhv;

    LoadPredicate(
            Term::Ptr lhv,
            Term::Ptr rhv);

    LoadPredicate(
            Term::Ptr lhv,
            Term::Ptr rhv,
            SlotTracker* st);

};

} /* namespace borealis */

#endif /* LOADPREDICATE_H_ */
