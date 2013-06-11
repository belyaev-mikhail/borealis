/*
 * BinaryTerm.h
 *
 *  Created on: Jan 17, 2013
 *      Author: belyaev
 */

#ifndef BINARYTERM_H_
#define BINARYTERM_H_

#include "Term/Term.h"

namespace borealis {

class BinaryTerm: public borealis::Term {

    typedef BinaryTerm self;

    llvm::ArithType opcode;
    Term::Ptr lhv;
    Term::Ptr rhv;

    BinaryTerm(llvm::ArithType opcode, Term::Ptr lhv, Term::Ptr rhv):
        Term(
                lhv->hashCode() ^ rhv->hashCode(),
                "(" + lhv->getName() + " " + llvm::arithString(opcode) + " " + rhv->getName() + ")",
                type_id(*this)
        ), opcode(opcode), lhv(lhv), rhv(rhv) {};

public:

    BinaryTerm(const BinaryTerm&) = default;
    ~BinaryTerm();

    template<class Sub>
    auto accept(Transformer<Sub>* tr) const -> const self* {
        return new BinaryTerm(opcode, tr->transform(lhv), tr->transform(rhv));
    }

#include "Util/macros.h"
    virtual Z3ExprFactory::Dynamic toZ3(Z3ExprFactory& z3ef, ExecutionContext* ctx) const {
        typedef Z3ExprFactory::Bool Bool;
        typedef Z3ExprFactory::DynBV DynBV;

        auto lhvz3 = lhv->toZ3(z3ef, ctx);
        auto rhvz3 = rhv->toZ3(z3ef, ctx);

        if(lhvz3.isBool() && rhvz3.isBool()) {
            auto lhv = lhvz3.to<Bool>().getUnsafe();
            auto rhv = rhvz3.to<Bool>().getUnsafe();

            switch(opcode) {
            case llvm::ArithType::BAND:
            case llvm::ArithType::LAND: return lhv && rhv;
            case llvm::ArithType::BOR:
            case llvm::ArithType::LOR:  return lhv || rhv;
            case llvm::ArithType::XOR:  return lhv ^  rhv;
            default: BYE_BYE(Z3ExprFactory::Dynamic,
                             "Unsupported logic opcode: " + llvm::arithString(opcode));
            }
        }

        if(lhvz3.is<DynBV>() && rhvz3.is<DynBV>()) {
            auto lhv = lhvz3.to<DynBV>().getUnsafe();
            auto rhv = rhvz3.to<DynBV>().getUnsafe();

            switch(opcode) {
            case llvm::ArithType::ADD:  return lhv +  rhv;
            case llvm::ArithType::BAND: return lhv &  rhv;
            case llvm::ArithType::BOR:  return lhv |  rhv;
            case llvm::ArithType::DIV:  return lhv /  rhv;
            case llvm::ArithType::SHL:  return lhv << rhv;
            case llvm::ArithType::MUL:  return lhv *  rhv;
            case llvm::ArithType::REM:  return lhv %  rhv;
            case llvm::ArithType::ASHR: return lhv >> rhv;
            case llvm::ArithType::SUB:  return lhv -  rhv;
            case llvm::ArithType::XOR:  return lhv ^  rhv;
            case llvm::ArithType::LSHR: return lhv.lshr(rhv);
            default: BYE_BYE(Z3ExprFactory::Dynamic,
                             "Unsupported bv opcode: " + llvm::arithString(opcode));
            }
        }

        BYE_BYE(Z3ExprFactory::Dynamic, "Unreachable!");
    }
#include "Util/unmacros.h"

    virtual bool equals(const Term* other) const {
        if (const BinaryTerm* that = llvm::dyn_cast<BinaryTerm>(other)) {
            return  Term::equals(other) &&
                    that->opcode == opcode &&
                    *that->lhv == *lhv &&
                    *that->rhv == *rhv;
        } else return false;
    }

    llvm::ArithType getOpcode() const { return opcode; }
    Term::Ptr getLhv() const { return lhv; }
    Term::Ptr getRhv() const { return rhv; }

    static bool classof(const BinaryTerm*) {
        return true;
    }

    static bool classof(const Term* t) {
        return t->getTermTypeId() == type_id<self>();
    }

    virtual Type::Ptr getTermType() const {
        auto& tf = TypeFactory::getInstance();
        return tf.merge(lhv->getTermType(), rhv->getTermType());
    }

    friend class TermFactory;
};

} /* namespace borealis */

#endif /* BINARYTERM_H_ */
