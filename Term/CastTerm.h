/*
 * CastTerm.h
 *
 *  Created on: Jul 03, 2013
 *      Author: sam
 */

#ifndef CASTTERM_H_
#define CASTTERM_H_

#include "Logging/tracer.hpp"
#include "Term/Term.h"

namespace borealis {

/** protobuf -> Term/CastTerm.proto
import "Term/Term.proto";
import "Util/CastType.proto";

package borealis.proto;

message CastTerm {
    extend borealis.proto.Term {
        optional CastTerm ext = $COUNTER_TERM;
    }

    optional CastType opcode = 1;
    optional Term rhv = 2;
}

**/
class CastTerm: public borealis::Term {

    llvm::CastType opcode;
    Term::Ptr rhv;

    CastTerm(llvm::CastType opcode, Term::Ptr rhv, Type::Ptr to):
        Term(
            class_tag(*this),
            to,
            "(" + TypeUtils::toString(*to)
            + ") " + rhv->getName()
        ), opcode(opcode), rhv(rhv) {};

public:

    MK_COMMON_TERM_IMPL(CastTerm);

    Term::Ptr getRhv() const { return rhv; }
    llvm::CastType getOpcode() const { return opcode; }

    template<class Sub>
    auto accept(Transformer<Sub>* tr) const -> Term::Ptr {
        auto _rhv = tr->transform(rhv);
        auto _type = getTermType(tr->FN.Type, _rhv);
        TERM_KILLED(_rhv);
        TERM_ON_CHANGED(
            rhv != _rhv,
            new Self( opcode, _rhv, _type )
        );
    }

    virtual bool equals(const Term* other) const override {
        if (const Self* that = llvm::dyn_cast_or_null<Self>(other)) {
            return Term::equals(other) &&
                    that->opcode == opcode &&
                    *that->rhv == *rhv;
        } else return false;
    }

    virtual size_t hashCode() const override {
        return util::hash::defaultHasher()(Term::hashCode(), rhv);
    }

    static Type::Ptr getTermType(TypeFactory::Ptr TyF, Term::Ptr rhv) {
        util::use(TyF);
        return rhv->getType();
    }


    static Type::Ptr resultTypeForCast(llvm::CastType opCode, Type::Ptr from);
    static llvm::CastType castForTypes(Type::Ptr from, Type::Ptr to);

};

#include "Util/macros.h"

template<class Impl>
struct SMTImpl<Impl, CastTerm> {
    static Dynamic<Impl> doit(
            const CastTerm* t,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>* ctx) {

        TRACE_FUNC;
        USING_SMT_IMPL(Impl);

        auto rhvz3 = SMT<Impl>::doit(t->getRhv(), ef, ctx);

        switch(t->getOpcode()) {
        case llvm::CastType::SIntToFloat: {
            auto rhvi = rhvz3.template to<Integer>().getUnsafe();
            return Dynamic::template convert<Integer, Real>(rhvi, true);
        }
        case llvm::CastType::SLongToFloat: {
            auto rhvl = rhvz3.template to<Long>().getUnsafe();
            return Dynamic::template convert<Long, Real>(rhvl, true);
        }
        case llvm::CastType::UIntToFloat: {
            auto rhvi = rhvz3.template to<Integer>().getUnsafe();
            return Dynamic::template convert<Integer, Real>(rhvi, false);
        }
        case llvm::CastType::ULongToFloat: {
            auto rhvl = rhvz3.template to<Long>().getUnsafe();
            return Dynamic::template convert<Long, Real>(rhvl, false);
        }
        case llvm::CastType::FloatToSInt: {
            auto rhvr = rhvz3.template to<Real>().getUnsafe();
            return Dynamic::template convert<Real, Integer>(rhvr, true);
        }
        case llvm::CastType::FloatToSLong: {
            auto rhvr = rhvz3.template to<Real>().getUnsafe();
            return Dynamic::template convert<Real, Long>(rhvr, true);
        }
        case llvm::CastType::FloatToUInt: {
            auto rhvr = rhvz3.template to<Real>().getUnsafe();
            return Dynamic::template convert<Real, Integer>(rhvr, false);
        }
        case llvm::CastType::FloatToULong: {
            auto rhvr = rhvz3.template to<Real>().getUnsafe();
            return Dynamic::template convert<Real, Long>(rhvr, false);
        }
        case llvm::CastType::LongToInt: {
            auto rhvl = rhvz3.template to<Long>().getUnsafe();
            return Dynamic::template convert<Long, Integer>(rhvl, true);
        }
        case llvm::CastType::IntToSLong: {
            auto rhvi = rhvz3.template to<Integer>().getUnsafe();
            return Dynamic::template convert<Integer, Long>(rhvi, true);
        }
        case llvm::CastType::IntToULong: {
            auto rhvi = rhvz3.template to<Integer>().getUnsafe();
            return Dynamic::template convert<Integer, Long>(rhvi, false);
        }
        case llvm::CastType::NoCast: {
            return rhvz3;
        }
        default: BYE_BYE(Dynamic, "Unreachable!!!");
        }
    }
};
#include "Util/unmacros.h"

} /* namespace borealis */

#endif /* CASTTERM_H_ */
