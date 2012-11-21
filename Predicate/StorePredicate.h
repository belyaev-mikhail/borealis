/*
 * StorePredicate.h
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#ifndef STOREPREDICATE_H_
#define STOREPREDICATE_H_

#include <llvm/Value.h>

#include "Predicate.h"

namespace borealis {

class PredicateFactory;

class StorePredicate: public Predicate {

public:

    virtual Predicate::Key getKey() const;

    virtual Dependee getDependee() const;
    virtual DependeeSet getDependees() const;

    virtual z3::expr toZ3(Z3ExprFactory& z3ef) const;

    static bool classof(const Predicate* p) {
        return p->getPredicateTypeId() == type_id<StorePredicate>();
    }

    static bool classof(const StorePredicate* /* p */) {
        return true;
    }

    template<class SubClass>
    const StorePredicate* accept(Transformer<SubClass>* t) {
        return new StorePredicate(
                t->transform(lhv),
                t->transform(rhv));
    }

    friend class PredicateFactory;

private:

    const Term::Ptr lhv;
    const Term::Ptr rhv;

    StorePredicate(
            Term::Ptr lhv,
            Term::Ptr rhv);

    StorePredicate(
            Term::Ptr lhv,
            Term::Ptr rhv,
            SlotTracker* st);

};

} /* namespace borealis */

#endif /* STOREPREDICATE_H_ */
