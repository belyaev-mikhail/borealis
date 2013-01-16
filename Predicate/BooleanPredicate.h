/*
 * BooleanPredicate.h
 *
 *  Created on: Sep 26, 2012
 *      Author: ice-phoenix
 */

#ifndef BOOLEANPREDICATE_H_
#define BOOLEANPREDICATE_H_

#include "Predicate.h"

namespace borealis {

class PredicateFactory;

class BooleanPredicate: public Predicate {

public:

	virtual Predicate::Key getKey() const;

	virtual logic::Bool toZ3(Z3ExprFactory& z3ef, ExecutionContext* = nullptr) const;

    static bool classof(const Predicate* p) {
        return p->getPredicateTypeId() == type_id<BooleanPredicate>();
    }

    static bool classof(const BooleanPredicate* /* p */) {
        return true;
    }

    template<class SubClass>
    const BooleanPredicate* accept(Transformer<SubClass>* t) const {
        return new BooleanPredicate(
                t->transform(v),
                t->transform(b));
    }

    virtual bool equals(const Predicate* other) const;
    virtual size_t hashCode() const;

	friend class PredicateFactory;

private:

	const Term::Ptr v;
	const Term::Ptr b;

	BooleanPredicate(
	        Term::Ptr v,
	        Term::Ptr b);

    BooleanPredicate(
            Term::Ptr v,
            bool b,
            SlotTracker* st);
    BooleanPredicate(
            PredicateType type,
            Term::Ptr v,
            bool b,
            SlotTracker* st);

};

} /* namespace borealis */

#endif /* BOOLEANPREDICATE_H_ */
