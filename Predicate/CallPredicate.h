/*
 * CallPredicate.h
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#ifndef CALLPREDICATE_H_
#define CALLPREDICATE_H_

#include "Config/config.h"
#include "Predicate/Predicate.h"
#include "Logging/tracer.hpp"


namespace borealis {

/** protobuf -> Predicate/CallPredicate.proto
import "Predicate/Predicate.proto";
import "Term/Term.proto";

package borealis.proto;

message CallPredicate {
    extend borealis.proto.Predicate {
        optional CallPredicate ext = $COUNTER_PRED;
    }

    optional Term funName = 1;
    optional Term result = 2;
    repeated Term args = 3;
}

**/
class CallPredicate: public borealis::Predicate {

    CallPredicate(
            Term::Ptr funName,
            Term::Ptr result,
            const std::vector<Term::Ptr>& args,
            const Locus& loc,
            PredicateType type = PredicateType::STATE);

public:

    MK_COMMON_PREDICATE_IMPL(CallPredicate);

    Term::Ptr getResult() const;
    auto getArgs() const -> decltype(util::viewContainer(ops));
    Term::Ptr getFunctionName() const;


    template<class SubClass>
    Predicate::Ptr accept(Transformer<SubClass>* t) const {
        TRACE_FUNC;
        auto&& _funName = t->transform(getFunctionName());
        Term::Ptr _res = nullptr;
        if(getResult() != nullptr)
            _res = t->transform(getResult());
        auto&& _args = getArgs().map(
            [&](auto&& d) { return t->transform(d); }
        );
        auto&& _loc = getLocation();
        auto&& _type = getType();
        PREDICATE_ON_CHANGED(
            getFunctionName() != _funName || getResult() != _res || not util::equal(getArgs(), _args, ops::equals_to),
            new Self(_funName, _res, _args.toVector(), _loc, _type )
        );
    }

};

#include "Util/macros.h"
template<class Impl>
struct SMTImpl<Impl, CallPredicate> {
    static Bool<Impl> doit(
            const CallPredicate*,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>* ctx) {
        TRACE_FUNC;

        USING_SMT_IMPL(Impl);

        ASSERTC(ctx != nullptr);

        return ef.getTrue();
    }
};
#include "Util/unmacros.h"

} /* namespace borealis */

#endif /* CALLPREDICATE_H_ */
