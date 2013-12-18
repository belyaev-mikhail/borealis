/*
 * WritePropertyPredicate.h
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#ifndef WRITEPROPERTYPREDICATE_H_
#define WRITEPROPERTYPREDICATE_H_

#include "Predicate/Predicate.h"

namespace borealis {

/** protobuf -> Predicate/WritePropertyPredicate.proto
import "Predicate/Predicate.proto";
import "Term/Term.proto";

package borealis.proto;

message WritePropertyPredicate {
    extend borealis.proto.Predicate {
        optional WritePropertyPredicate ext = $COUNTER_PRED;
    }

    optional Term propName = 1;
    optional Term lhv = 2;
    optional Term rhv = 3;
}

**/
class WritePropertyPredicate: public borealis::Predicate {

    Term::Ptr propName;
    Term::Ptr lhv;
    Term::Ptr rhv;

    WritePropertyPredicate(
            Term::Ptr propName,
            Term::Ptr lhv,
            Term::Ptr rhv,
            const Locus& loc,
            PredicateType type = PredicateType::STATE);

public:

    MK_COMMON_PREDICATE_IMPL(WritePropertyPredicate);

    Term::Ptr getPropertyName() const { return propName; }
    Term::Ptr getLhv() const { return lhv; }
    Term::Ptr getRhv() const { return rhv; }

    template<class SubClass>
    Predicate::Ptr accept(Transformer<SubClass>* t) const {
        auto _propName = t->transform(propName);
        auto _lhv = t->transform(lhv);
        auto _rhv = t->transform(rhv);
        auto _loc = location;
        auto _type = type;
        PREDICATE_ON_CHANGED(
            propName != _propName || lhv != _lhv || rhv != _rhv,
            new Self( _propName, _lhv, _rhv, _loc, _type )
        );
    }

    virtual bool equals(const Predicate* other) const override;
    virtual size_t hashCode() const override;

};

#include "Util/macros.h"
template<class Impl>
struct SMTImpl<Impl, WritePropertyPredicate> {
    static Bool<Impl> doit(
            const WritePropertyPredicate* p,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>* ctx) {
        TRACE_FUNC;

        USING_SMT_IMPL(Impl);

        ASSERTC(ctx != nullptr);

        ASSERT(llvm::isa<OpaqueStringConstantTerm>(p->getPropertyName()),
               "Property write with non-string property name");
        auto* propName = llvm::cast<OpaqueStringConstantTerm>(p->getPropertyName());
        auto strPropName = propName->getValue();

        auto l = SMT<Impl>::doit(p->getLhv(), ef, ctx).template to<Pointer>();
        ASSERT(!l.empty(), "Property write with a non-pointer value");
        auto lp = l.getUnsafe();

        auto r = SMT<Impl>::doit(p->getRhv(), ef, ctx);

        ctx->writeProperty(strPropName, lp, r);

        return ef.getTrue();
    }
};
#include "Util/unmacros.h"

} /* namespace borealis */

#endif /* WRITEPROPERTYPREDICATE_H_ */
