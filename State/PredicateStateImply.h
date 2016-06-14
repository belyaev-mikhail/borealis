/*
 * PredicateStateImply.h
 *
 *  Created on: Apr 30, 2013
 *      Author: ice-phoenix
 */

#ifndef PREDICATESTATEIMPLY_H_
#define PREDICATESTATEIMPLY_H_

#include "State/PredicateState.h"

namespace borealis {

/** protobuf -> State/PredicateStateImply.proto
import "State/PredicateState.proto";

package borealis.proto;

message PredicateStateImply {
    extend borealis.proto.PredicateState {
        optional PredicateStateImply ext = 23;
    }

    optional PredicateState base = 1;
    optional PredicateState curr = 2;
}

**/
class PredicateStateImply :
        public PredicateState {

public:

    MK_COMMON_STATE_IMPL(PredicateStateImply);

    PredicateState::Ptr getBase() const;
    PredicateState::Ptr getCurr() const;

    PredicateState::Ptr swapBase(PredicateState::Ptr newBase) const;
    PredicateState::Ptr swapCurr(PredicateState::Ptr newCurr) const;

    virtual PredicateState::Ptr addPredicate(Predicate::Ptr pred) const override;

    virtual PredicateState::Ptr addVisited(const Locus& locus) const override;
    virtual bool hasVisited(std::initializer_list<Locus> loci) const override;
    virtual bool hasVisitedFrom(Loci& visited) const override;

    virtual Loci getVisited() const override;

    virtual PredicateState::Ptr fmap(FMapper f) const override;
    virtual PredicateState::Ptr reverse() const override;

    virtual std::pair<PredicateState::Ptr, PredicateState::Ptr> splitByTypes(
            std::initializer_list<PredicateType> types) const override;
    virtual PredicateState::Ptr sliceOn(PredicateState::Ptr on) const override;
    virtual PredicateState::Ptr simplify() const override;

    virtual bool isEmpty() const override;
    virtual unsigned int size() const override;

    virtual bool equals(const PredicateState* other) const override;

    virtual std::string toString() const override;
    virtual borealis::logging::logstream& dump(borealis::logging::logstream& s) const override;

private:

    PredicateState::Ptr base;
    PredicateState::Ptr curr;

    PredicateStateImply(PredicateState::Ptr base, PredicateState::Ptr curr);

    SelfPtr fmap_(FMapper f) const;

};

template<class Impl>
struct SMTImpl<Impl, PredicateStateImply> {
    static Bool<Impl> doit(
            const PredicateStateImply* s,
            ExprFactory<Impl>& ef,
            ExecutionContext<Impl>* ctx,
            bool pathMode = false) {
        TRACE_FUNC;

        auto res = ef.getTrue();
        res = res && (not(SMT<Impl>::doit(s->getBase(), ef, ctx, pathMode)) || SMT<Impl>::doit(s->getCurr(), ef, ctx, pathMode));
        return res;
    }
};

} /* namespace borealis */

#endif /* PREDICATESTATEIMPLY_H_ */
