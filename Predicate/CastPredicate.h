/*
 * CastPredicate.h
 *
 *  Created on: Apr 21, 2014
 *      Author: sam
 */

#ifndef CASTPREDICATE_H_
#define CASTPREDICATE_H_

#include "Predicate/Predicate.h"

namespace borealis {

/** protobuf -> Predicate/CastPredicate.proto
import "Predicate/Predicate.proto";
import "Term/Term.proto";

package borealis.proto;

message CastPredicate {
    extend borealis.proto.Predicate {
        optional CastPredicate ext = $COUNTER_PRED;
    }

    optional Term lhv = 1;
    optional Term rhv = 2;
}

**/
class CastPredicate: public borealis::Predicate {

    Term::Ptr lhv;
    Term::Ptr rhv;

    CastPredicate(
            Term::Ptr lhv,
            Term::Ptr rhv,
            const Locus& loc);

public:

    MK_COMMON_PREDICATE_IMPL(CastPredicate);

    Term::Ptr getLhv() const { return lhv; }
    Term::Ptr getRhv() const { return rhv; }
    Type::Ptr getToType() const { return lhv->getType(); }
    Type::Ptr getFromType() const { return rhv->getType(); }

    template<class SubClass>
    Predicate::Ptr accept(Transformer<SubClass>* t) const {
        auto _lhv = t->transform(lhv);
        auto _rhv = t->transform(rhv);
        auto _loc = location;
        PREDICATE_ON_CHANGED(
            lhv != _lhv || rhv != _rhv,
            new Self( _lhv, _rhv, _loc )
        );
    }

    virtual bool equals(const Predicate* other) const override;
    virtual size_t hashCode() const override;

};

template<class Impl>
struct SMTImpl<Impl, CastPredicate> {

public:
    static Bool<Impl> doit(
            const CastPredicate* p,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>* ctx) {
        TRACE_FUNC;
        USING_SMT_IMPL(Impl);

        using llvm::isa;

        auto to = p->getToType();
        auto from = p->getFromType();

        auto lhvz3 = SMT<Impl>::doit(p->getLhv(), ef, ctx);
        auto rhvz3 = SMT<Impl>::doit(p->getRhv(), ef, ctx);

        if (isa<type::Bool>(to) && not isa<type::Bool>(from)) {
            auto rhvi = rhvz3.template to<Long>().getUnsafe();
            return ef.getIntConst(0) != rhvi;
        } else if (not isa<type::Bool>(to) && isa<type::Bool>(from)) {
            auto lhvi = lhvz3.template to<Long>().getUnsafe();
            return lhvi != ef.getIntConst(0);
        } else if (isa<type::Float>(to) && isa<type::Integer>(from)) {
            auto lhvr = lhvz3.template to<Real>().getUnsafe();
            auto rhvi = rhvz3.template to<Integer>().getUnsafe();
            auto isSigned = (TypeUtils::isSigned(from) == llvm::Signedness::Signed);
            auto rhvr = Dynamic::template convert<Integer, Real>(rhvi, isSigned);
            return lhvr == rhvr;
        } else if (isa<type::Integer>(to) && isa<type::Float>(from)) {
            auto lhvi = lhvz3.template to<Integer>().getUnsafe();
            auto rhvr = rhvz3.template to<Real>().getUnsafe();
            auto isSigned = (TypeUtils::isSigned(to) == llvm::Signedness::Signed);
            auto rhvi = Dynamic::template convert<Real, Integer>(rhvr, isSigned);
            return lhvi == rhvi;
        } else if (isa<type::Integer>(to) && isa<type::Integer>(from)) {
            auto isFromLong = ( ef.sizeForType(from) > 32 );
            auto isToLong = ( ef.sizeForType(to) > 32 );
            if (isFromLong && not isToLong) {
                auto lhvi = lhvz3.template to<Integer>().getUnsafe();
                auto rhvl = rhvz3.template to<Long>().getUnsafe();
                auto rhvi = Dynamic::template convert<Long, Integer>(rhvl);
                return lhvi == rhvi;
            } else if (not isFromLong && isToLong) {
                auto lhvl = lhvz3.template to<Long>().getUnsafe();
                auto rhvi = rhvz3.template to<Integer>().getUnsafe();
                auto isSigned = (TypeUtils::isSigned(from) == llvm::Signedness::Signed);
                auto rhvl = Dynamic::template convert<Integer, Long>(rhvi, isSigned);
                return lhvl == rhvl;
            }
        }
        return lhvz3 == rhvz3;
    }
};

} /* namespace borealis */

#endif /* CASTPREDICATE_H_ */
