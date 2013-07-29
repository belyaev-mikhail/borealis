/*
 * ValueTerm.h
 *
 *  Created on: Nov 16, 2012
 *      Author: ice-phoenix
 */

#ifndef VALUETERM_H_
#define VALUETERM_H_

#include <llvm/Value.h>

#include "Term/Term.h"
#include "Util/slottracker.h"

namespace borealis {

/** protobuf -> Term/ValueTerm.proto
import "Term/Term.proto";

package borealis.proto;

message ValueTerm {
    extend borealis.proto.Term {
        optional ValueTerm ext = 33;
    }
}

**/
class ValueTerm: public borealis::Term {

    typedef std::unique_ptr<ValueTerm> SelfPtr;

    ValueTerm(Type::Ptr type, const std::string& name) :
        Term(
            class_tag(*this),
            type,
            name
        ) {};

public:

    MK_COMMON_TERM_IMPL(ValueTerm);

    template<class Sub>
    auto accept(Transformer<Sub>*) const -> const Self* {
        return new Self( *this );
    }

    Term::Ptr withNewName(const std::string& name) const {
        auto res = SelfPtr{ new Self{ *this } };
        res->name = name;
        return Term::Ptr{ res.release() };
    }

};

template<class Impl>
struct SMTImpl<Impl, ValueTerm> {
    static Dynamic<Impl> doit(
            const ValueTerm* t,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>*) {
        return ef.getVarByTypeAndName(t->getType(), t->getName());
    }
};

} /* namespace borealis */

#endif /* VALUETERM_H_ */
