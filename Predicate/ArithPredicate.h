/*
 * ArithPredicate.h
 *
 *  Created on: Dec 28, 2012
 *      Author: ice-phoenix
 */

#ifndef ARITHPREDICATE_H_
#define ARITHPREDICATE_H_

#include "Predicate/Predicate.h"

namespace borealis {

class ArithPredicate: public borealis::Predicate {

public:

    virtual logic::Bool toZ3(Z3ExprFactory& z3ef, ExecutionContext* = nullptr) const;

    static bool classof(const Predicate* p) {
        return p->getPredicateTypeId() == type_id<ArithPredicate>();
    }

    static bool classof(const ArithPredicate* /* p */) {
        return true;
    }

    template<class SubClass>
    const ArithPredicate* accept(Transformer<SubClass>* t) const {
        return new ArithPredicate(
                this->type,
                t->transform(lhv),
                t->transform(op1),
                t->transform(op2),
                opCode);
    }

    virtual bool equals(const Predicate* other) const;
    virtual size_t hashCode() const;

    friend class PredicateFactory;

private:

    const Term::Ptr lhv;
    const Term::Ptr op1;
    const Term::Ptr op2;
    const llvm::ArithType opCode;
    const std::string _opCode;

    ArithPredicate(
            PredicateType type,
            Term::Ptr lhv,
            Term::Ptr op1,
            Term::Ptr op2,
            llvm::ArithType opCode);
    ArithPredicate(
            Term::Ptr lhv,
            Term::Ptr op1,
            Term::Ptr op2,
            llvm::ArithType opCode,
            SlotTracker* st,
            PredicateType type = PredicateType::STATE);

};

} /* namespace borealis */

#endif /* ARITHPREDICATE_H_ */
