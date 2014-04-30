/*
 * UnaryTerm.h
 *
 *  Created on: Jan 17, 2013
 *      Author: belyaev
 */

#ifndef UNARYTERM_H_
#define UNARYTERM_H_

#include "Term/Term.h"

namespace borealis {

/** protobuf -> Term/UnaryTerm.proto
import "Term/Term.proto";
import "Util/UnaryArithType.proto";

package borealis.proto;

message UnaryTerm {
    extend borealis.proto.Term {
        optional UnaryTerm ext = $COUNTER_TERM;
    }

    optional UnaryArithType opcode = 1;
    optional Term rhv = 2;
}

**/
class UnaryTerm: public borealis::Term {

    llvm::UnaryArithType opcode;
    Term::Ptr rhv;

    UnaryTerm(Type::Ptr type, llvm::UnaryArithType opcode, Term::Ptr rhv):
        Term(
            class_tag(*this),
            type,
            llvm::unaryArithString(opcode) + "(" + rhv->getName() + ")"
        ), opcode(opcode), rhv(rhv) {};

public:

    MK_COMMON_TERM_IMPL(UnaryTerm);

    llvm::UnaryArithType getOpcode() const { return opcode; }
    Term::Ptr getRhv() const { return rhv; }

    template<class Sub>
    auto accept(Transformer<Sub>* tr) const -> Term::Ptr {
        auto _rhv = tr->transform(rhv);
        auto _type = getTermType(tr->FN.Type, _rhv);
        TERM_ON_CHANGED(
            rhv != _rhv,
            new Self( _type, opcode, _rhv )
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
        return util::hash::defaultHasher()(Term::hashCode(), opcode, rhv);
    }

    static Type::Ptr getTermType(TypeFactory::Ptr, Term::Ptr rhv) {
        return rhv->getType();
    }

};

#include "Util/macros.h"
template<class Impl>
struct SMTImpl<Impl, UnaryTerm> {
    static Dynamic<Impl> doit(
            const UnaryTerm* t,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>* ctx) {

        USING_SMT_IMPL(Impl);

        auto rhvz3 = SMT<Impl>::doit(t->getRhv(), ef, ctx);

        auto rhvi = rhvz3.template to<Integer>();
        if (not rhvi.empty()) {
            auto rhv = rhvi.getUnsafe();
            switch (t->getOpcode()) {
            case llvm::UnaryArithType::BNOT:    return ~rhv;
            case llvm::UnaryArithType::NEG:     return -rhv;
            default:
                BYE_BYE(Dynamic, "Unsupported bv opcode: " +
                        llvm::unaryArithString(t->getOpcode()));
            }
        }

        auto rhvr = rhvz3.template to<Real>();
        if (not rhvr.empty()) {
            auto rhv = rhvr.getUnsafe();
            switch (t->getOpcode()) {
            case llvm::UnaryArithType::BNOT:    return ~rhv;
            case llvm::UnaryArithType::NEG:     return -rhv;
            default:
                BYE_BYE(Dynamic, "Unsupported float opcode: " +
                        llvm::unaryArithString(t->getOpcode()));
            }
        }

        auto rhvb = rhvz3.template to<Bool>();
        if (not rhvb.empty()) {
            auto rhv = rhvb.getUnsafe();
            if (t->getOpcode() == llvm::UnaryArithType::NOT)
                return !rhv;
            else
                BYE_BYE(Dynamic, "Unsupported bool opcode: " +
                        llvm::unaryArithString(t->getOpcode()));
        }

        BYE_BYE(Dynamic, "Unreachable");
    }
};
#include "Util/unmacros.h"

} /* namespace borealis */

#endif /* UNARYTERM_H_ */
