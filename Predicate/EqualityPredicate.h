/*
 * EqualityPredicate.h
 *
 *  Created on: Oct 26, 2012
 *      Author: ice-phoenix
 */

#ifndef EQUALITYPREDICATE_H_
#define EQUALITYPREDICATE_H_

#include "Predicate/Predicate.h"

namespace borealis {

/** protobuf -> Predicate/EqualityPredicate.proto
import "Predicate/Predicate.proto";
import "Term/Term.proto";

package borealis.proto;

message EqualityPredicate {
    extend borealis.proto.Predicate {
        optional EqualityPredicate ext = $COUNTER_PRED;
    }

    optional Term lhv = 1;
    optional Term rhv = 2;
}

**/
class EqualityPredicate: public borealis::Predicate {

    Term::Ptr lhv;
    Term::Ptr rhv;

    EqualityPredicate(
            Term::Ptr lhv,
            Term::Ptr rhv,
            const Locus& loc,
            PredicateType type = PredicateType::STATE);

public:

    MK_COMMON_PREDICATE_IMPL(EqualityPredicate);

    Term::Ptr getLhv() const { return lhv; }
    Term::Ptr getRhv() const { return rhv; }

    template<class SubClass>
    Predicate::Ptr accept(Transformer<SubClass>* t) const {
        auto _lhv = t->transform(lhv);
        auto _rhv = t->transform(rhv);
        auto _loc = location;
        auto _type = type;
        PREDICATE_KILLED(_rhv, _lhv);
        PREDICATE_ON_CHANGED(
            lhv != _lhv || rhv != _rhv,
            new Self( _lhv, _rhv, _loc, _type )
        );
    }

    virtual bool equals(const Predicate* other) const override;
    virtual size_t hashCode() const override;

};

template<class Impl>
struct SMTImpl<Impl, EqualityPredicate> {
    static Bool<Impl> doit(
            const EqualityPredicate* p,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>* ctx) {
        TRACE_FUNC;
        return SMT<Impl>::doit(p->getLhv(), ef, ctx) == SMT<Impl>::doit(p->getRhv(), ef, ctx);
    }
};

} /* namespace borealis */

#endif /* EQUALITYPREDICATE_H_ */
