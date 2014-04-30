/*
 * OpaqueConstantTerm.h
 *
 *  Created on: Jan 17, 2013
 *      Author: belyaev
 */

#ifndef OPAQUEFLOATINGCONSTANTTERM_H_
#define OPAQUEFLOATINGCONSTANTTERM_H_

#include "Term/Term.h"

namespace borealis {

/** protobuf -> Term/OpaqueFloatingConstantTerm.proto
import "Term/Term.proto";

package borealis.proto;

message OpaqueFloatingConstantTerm {
    extend borealis.proto.Term {
        optional OpaqueFloatingConstantTerm ext = $COUNTER_TERM;
    }

    optional double value = 1;
}

**/
class OpaqueFloatingConstantTerm: public borealis::Term {

    static constexpr double EPS = 4 * std::numeric_limits<double>::epsilon();

    double value;

    OpaqueFloatingConstantTerm(Type::Ptr type, double value):
        Term(
            class_tag(*this),
            type,
            util::toString(value)
        ), value(value) {};

public:

    MK_COMMON_TERM_IMPL(OpaqueFloatingConstantTerm);

    double getValue() const { return value; }

    template<class Sub>
    auto accept(Transformer<Sub>*) const -> Term::Ptr {
        return this->shared_from_this();
    }

    virtual bool equals(const Term* other) const override {
        if (const Self* that = llvm::dyn_cast_or_null<Self>(other)) {
            return Term::equals(other) &&
                    std::abs(that->value - value) < EPS;
        } else return false;
    }

    virtual size_t hashCode() const override {
        return util::hash::defaultHasher()(Term::hashCode(), value);
    }

};

template<class Impl>
struct SMTImpl<Impl, OpaqueFloatingConstantTerm> {
    static Dynamic<Impl> doit(
            const OpaqueFloatingConstantTerm* t,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>*) {
        double d = t->getValue();
        if (std::isnan(d)) {   //check double for NaN
            d = 0;
        } else if (std::isinf(d)) {  // check for infinity
            d = std::numeric_limits<double>::max();
        }
        return ef.getRealConst(d);
    }
};

} /* namespace borealis */

#endif /* OPAQUEFLOATINGCONSTANTTERM_H_ */
