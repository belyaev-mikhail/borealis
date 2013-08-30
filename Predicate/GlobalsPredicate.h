/*
 * GlobalsPredicate.h
 *
 *  Created on: Mar 13, 2013
 *      Author: ice-phoenix
 */

#ifndef GLOBALSPREDICATE_H_
#define GLOBALSPREDICATE_H_

#include "Protobuf/Gen/Predicate/GlobalsPredicate.pb.h"

#include "Predicate/Predicate.h"

namespace borealis {

/** protobuf -> Predicate/GlobalsPredicate.proto
import "Predicate/Predicate.proto";
import "Term/Term.proto";

package borealis.proto;

message GlobalsPredicate {
    extend borealis.proto.Predicate {
        optional GlobalsPredicate ext = 19;
    }

    repeated Term globals = 1;
}

**/
class GlobalsPredicate: public borealis::Predicate {

    typedef std::vector<Term::Ptr> Globals;

    const Globals globals;

    GlobalsPredicate(
            const Globals& globals,
            PredicateType type = PredicateType::STATE);

public:

    MK_COMMON_PREDICATE_IMPL(GlobalsPredicate);

    const Globals& getGlobals() const { return globals; }

    template<class SubClass>
    const Self* accept(Transformer<SubClass>* t) const {
        Globals transformedGlobals;
        transformedGlobals.reserve(globals.size());
        std::transform(globals.begin(), globals.end(), std::back_inserter(transformedGlobals),
            [&t](const Term::Ptr& e) { return t->transform(e); }
        );

        // XXX: Should be `Self{...}`, but clang++ crashes on that...
        return new Self(
            transformedGlobals,
            type
        );
    }

    virtual bool equals(const Predicate* other) const override;
    virtual size_t hashCode() const override;

};

#include "Util/macros.h"
template<class Impl>
struct SMTImpl<Impl, GlobalsPredicate> {
    static Bool<Impl> doit(
            const GlobalsPredicate* p,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>* ctx) {
        TRACE_FUNC;

        USING_SMT_IMPL(Impl)

        ASSERTC(ctx != nullptr);

        auto res = ef.getTrue();
        for (const auto& g : p->getGlobals()) {
            auto ge = SMT<Impl>::doit(g, ef, ctx).template to<Pointer>();
            ASSERT(!ge.empty(), "Encountered non-Pointer global value: " + g->getName());
            auto gp = ge.getUnsafe();
            res = res && gp == ctx->getDistinctPtr();
        }
        return res;
    }
};
#include "Util/unmacros.h"

} /* namespace borealis */

#endif /* GLOBALSPREDICATE_H_ */
