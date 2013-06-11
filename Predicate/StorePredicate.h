/*
 * StorePredicate.h
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#ifndef STOREPREDICATE_H_
#define STOREPREDICATE_H_

#include "Predicate/Predicate.h"

namespace borealis {

class StorePredicate: public borealis::Predicate {

public:

    virtual logic::Bool toZ3(Z3ExprFactory& z3ef, ExecutionContext*) const override;

    static bool classof(const Predicate* p) {
        return p->getPredicateTypeId() == type_id<StorePredicate>();
    }

    static bool classof(const StorePredicate* /* p */) {
        return true;
    }

    template<class SubClass>
    const StorePredicate* accept(Transformer<SubClass>* t) const {
        return new StorePredicate(
                t->transform(lhv),
                t->transform(rhv),
                this->type);
    }

    virtual bool equals(const Predicate* other) const override;
    virtual size_t hashCode() const override;

    friend class PredicateFactory;

private:

    Term::Ptr lhv;
    Term::Ptr rhv;

    StorePredicate(
            Term::Ptr lhv,
            Term::Ptr rhv,
            PredicateType type = PredicateType::STATE);

};

} /* namespace borealis */

#endif /* STOREPREDICATE_H_ */
