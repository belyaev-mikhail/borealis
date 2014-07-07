/*
 * TermFactory.h
 *
 *  Created on: Nov 19, 2012
 *      Author: ice-phoenix
 */

#ifndef TERMFACTORY_H_
#define TERMFACTORY_H_

#include <memory>

#include "Codegen/llvm.h"
#include "Term/Term.def"
#include "Type/TypeFactory.h"
#include "Util/slottracker.h"

#include "Util/macros.h"

namespace borealis {

class TermFactory {

    typedef std::vector<llvm::Value*> ValueVector;

public:

    typedef std::shared_ptr<TermFactory> Ptr;

    Term::Ptr getArgumentTerm(const llvm::Argument* arg, llvm::Signedness sign = llvm::Signedness::Unknown) {
        ASSERT(st, "Missing SlotTracker");

        return Term::Ptr{
            new ArgumentTerm(
                TyF->cast(arg->getType(), sign),
                arg->getArgNo(),
                st->getLocalName(arg)
            )
        };
    }

    Term::Ptr getStringArgumentTerm(llvm::Argument* arg, llvm::Signedness sign = llvm::Signedness::Unknown) {
        ASSERT(st, "Missing SlotTracker");

        return Term::Ptr{
            new ArgumentTerm(
                TyF->cast(arg->getType(), sign),
                arg->getArgNo(),
                st->getLocalName(arg),
                ArgumentKind::STRING
            )
        };
    }

    Term::Ptr getConstTerm(llvm::Constant* c, llvm::Signedness sign = llvm::Signedness::Unknown) {
        ASSERT(st, "Missing SlotTracker");

        using namespace llvm;
        using borealis::util::tail;
        using borealis::util::view;

        if (auto* cE = dyn_cast<ConstantExpr>(c)) {
            auto opcode = cE->getOpcode();

            if (opcode >= Instruction::CastOpsBegin && opcode <= Instruction::CastOpsEnd) {
                return getValueTerm(cE->getOperand(0));
            } else if (opcode == Instruction::GetElementPtr) {
                auto* base = cE->getOperand(0);
                ValueVector idxs;
                idxs.reserve(cE->getNumOperands() - 1);
                for (auto& i : tail(view(cE->op_begin(), cE->op_end()))) {
                    idxs.push_back(i);
                }
                return getGepTerm(base, idxs);
            }

        } else if (auto* null = dyn_cast<ConstantPointerNull>(c)) {
            return getNullPtrTerm(null);

        } else if (auto* cInt = dyn_cast<ConstantInt>(c)) {
            auto size = cInt->getType()->getPrimitiveSizeInBits();
            if (size == 1) {
                if (cInt->isOne()) return getTrueTerm();
                else if (cInt->isZero()) return getFalseTerm();
            } else {
                if (size > 32)
                    return getLongTerm(cInt->getValue().getSExtValue(), sign);
                else
                    return getIntTerm(cInt->getValue().getSExtValue(), sign);
            }

        } else if (auto* cFP = dyn_cast<ConstantFP>(c)) {
            auto& fp = cFP->getValueAPF();

            if (&fp.getSemantics() == &APFloat::IEEEsingle) {
                return getRealTerm(fp.convertToFloat());
            } else if (&fp.getSemantics() == &APFloat::IEEEdouble) {
                return getRealTerm(fp.convertToDouble());
            } else {
                BYE_BYE(Term::Ptr, "Unsupported semantics of APFloat");
            }

        } else if (auto* undef = dyn_cast<UndefValue>(c)) {
            return getUndefTerm(undef);

        } else if (auto* gv = dyn_cast<GlobalVariable>(c)) {
            // These guys should be processed separately by SeqDataPredicate
            // XXX: Keep in sync with FactoryNest
            return Term::Ptr{
                new ValueTerm(
                    TyF->cast(gv->getType()),
                    st->getLocalName(gv)
                )
            };

        }

        return Term::Ptr{
            new ConstTerm(
                TyF->cast(c->getType()),
                st->getLocalName(c)
            )
        };
    }

    Term::Ptr getNullPtrTerm() {
        return Term::Ptr{
            new OpaqueNullPtrTerm(TyF->getUnknownType())
        };
    }

    Term::Ptr getNullPtrTerm(llvm::ConstantPointerNull* n) {
        return Term::Ptr{
            new OpaqueNullPtrTerm(TyF->cast(n->getType()))
        };
    }

    Term::Ptr getUndefTerm(llvm::UndefValue* u) {
        return Term::Ptr{
            new OpaqueUndefTerm(TyF->cast(u->getType()))
        };
    }

    Term::Ptr getInvalidPtrTerm() {
        return Term::Ptr{
            new OpaqueInvalidPtrTerm(TyF->getUnknownType())
        };
    }

    Term::Ptr getBooleanTerm(bool b) {
        return Term::Ptr{
            new OpaqueBoolConstantTerm(
                TyF->getBool(), b
            )
        };
    }

    Term::Ptr getTrueTerm() {
        return getBooleanTerm(true);
    }

    Term::Ptr getFalseTerm() {
        return getBooleanTerm(false);
    }

    Term::Ptr getIntTerm(long long i, size_t size,
                         llvm::Signedness sign = llvm::Signedness::Unknown) {
        return Term::Ptr{
            new OpaqueIntConstantTerm(
                TyF->getInteger(size, sign), i // XXX: 32 -> ???
            )
        };
    }

    Term::Ptr getIntTerm(long long i, llvm::Signedness sign = llvm::Signedness::Unknown) {
        return getIntTerm(i, 32, sign);
    }

    Term::Ptr getLongTerm(long long i, llvm::Signedness sign = llvm::Signedness::Unknown) {
        return getIntTerm(i, 64, sign);
    }

    Term::Ptr getRealTerm(double d) {
        return Term::Ptr{
            new OpaqueFloatingConstantTerm(
                TyF->getFloat(), d
            )
        };
    }

    Term::Ptr getReturnValueTerm(llvm::Function* F, llvm::Signedness sign = llvm::Signedness::Unknown) {
        ASSERT(st, "Missing SlotTracker");

        return Term::Ptr{
            new ReturnValueTerm(
                TyF->cast(F->getFunctionType()->getReturnType(), sign),
                F->getName().str()
            )
        };
    }

    Term::Ptr getValueTerm(llvm::Value* v, llvm::Signedness sign = llvm::Signedness::Unknown) {
        ASSERT(st, "Missing SlotTracker");
        using namespace llvm;

        if (auto* gv = dyn_cast<GlobalValue>(v))
            return getGlobalValueTerm(gv, sign);
        else if (auto* c = dyn_cast<Constant>(v))
            return getConstTerm(c, sign);
        else if (auto* arg = dyn_cast<Argument>(v))
            return getArgumentTerm(arg, sign);
        else
            return getLocalValueTerm(v, sign);
    }

    Term::Ptr getValueTerm(Type::Ptr type, const std::string& name) {
        return Term::Ptr{ new ValueTerm(type, name) };
    }

    Term::Ptr getGlobalValueTerm(llvm::GlobalValue* gv, llvm::Signedness sign = llvm::Signedness::Unknown) {
        return Term::Ptr{
            new ValueTerm(
                TyF->cast(gv->getType(), sign),
                st->getLocalName(gv),
                /* global = */true
            )
        };
    }

    Term::Ptr getLocalValueTerm(llvm::Value* v, llvm::Signedness sign = llvm::Signedness::Unknown) {
        return Term::Ptr{
            new ValueTerm(
                TyF->cast(v->getType(), sign),
                st->getLocalName(v)
            )
        };
    }

    Term::Ptr getTernaryTerm(Term::Ptr cnd, Term::Ptr tru, Term::Ptr fls) {
        return Term::Ptr{
            new TernaryTerm(
                TernaryTerm::getTermType(TyF, cnd, tru, fls),
                cnd, tru, fls
            )
        };
    }

    Term::Ptr getBinaryTerm(llvm::ArithType opc, Term::Ptr lhv, Term::Ptr rhv) {
        return Term::Ptr{
            new BinaryTerm(
                BinaryTerm::getTermType(TyF, lhv, rhv),
                opc, lhv, rhv
            )
        };
    }

    Term::Ptr getUnaryTerm(llvm::UnaryArithType opc, Term::Ptr rhv) {
        return Term::Ptr{
            new UnaryTerm(
                UnaryTerm::getTermType(TyF, rhv),
                opc, rhv
            )
        };
    }

    Term::Ptr getUnlogicLoadTerm(Term::Ptr rhv) {
        return Term::Ptr{
            new LoadTerm(
                TyF->getInteger(),
                rhv,
                /* retypable = */false
            )
        };
    }

    Term::Ptr getLoadTerm(Term::Ptr rhv) {
        return Term::Ptr{
            new LoadTerm(
                LoadTerm::getTermType(TyF, rhv),
                rhv
            )
        };
    }

    Term::Ptr getReadPropertyTerm(Type::Ptr type, Term::Ptr propName, Term::Ptr rhv) {
        return Term::Ptr{
            new ReadPropertyTerm(type, propName, rhv)
        };
    }

    Term::Ptr getGepTerm(Term::Ptr base, const std::vector<Term::Ptr>& shifts) {

        Type::Ptr tp = GepTerm::getGepChild(base->getType(), shifts);

        return Term::Ptr{
            new GepTerm{
                TyF->getPointer(tp),
                base,
                shifts
            }
        };
    }

    Term::Ptr getGepTerm(llvm::Value* base, const ValueVector& idxs) {
        ASSERT(st, "Missing SlotTracker");

        using namespace llvm;
        using borealis::util::take;
        using borealis::util::view;

        auto type = GetElementPtrInst::getGEPReturnType(base, idxs);
        ASSERT(!!type, "getGepTerm: type after GEP is funked up");

        return Term::Ptr{
            new GepTerm(
                TyF->cast(type),
                getValueTerm(base),
                util::viewContainer(idxs)
                   .map([this](llvm::Value* idx){ return getValueTerm(idx); })
                   .toVector()
            )
        };
    }

    Term::Ptr getCmpTerm(llvm::ConditionType opc, Term::Ptr lhv, Term::Ptr rhv) {
        return Term::Ptr{
            new CmpTerm(
                CmpTerm::getTermType(TyF, lhv, rhv),
                opc, lhv, rhv
            )
        };
    }

    Term::Ptr getOpaqueVarTerm(const std::string& name) {
        return Term::Ptr{
            new OpaqueVarTerm(TyF->getUnknownType(), name)
        };
    }

    Term::Ptr getOpaqueBuiltinTerm(const std::string& name) {
        return Term::Ptr{
            new OpaqueBuiltinTerm(TyF->getUnknownType(), name)
        };
    }

    Term::Ptr getOpaqueConstantTerm(int v) {
            return Term::Ptr{
                new OpaqueIntConstantTerm(TyF->getInteger(32), v)
            };
        }

    Term::Ptr getOpaqueConstantTerm(long long v) {
        return Term::Ptr{
            new OpaqueIntConstantTerm(TyF->getInteger(64), v)
        };
    }

    Term::Ptr getOpaqueConstantTerm(double v) {
        return Term::Ptr{
            new OpaqueFloatingConstantTerm(TyF->getFloat(), v)
        };
    }

    Term::Ptr getOpaqueConstantTerm(bool v) {
        return Term::Ptr{
            new OpaqueBoolConstantTerm(TyF->getBool(), v)
        };
    }

    Term::Ptr getOpaqueConstantTerm(const char* v) {
        return Term::Ptr{
            new OpaqueStringConstantTerm(
                TyF->getUnknownType(), std::string{v}
            )
        };
    }

    Term::Ptr getOpaqueConstantTerm(const std::string& v) {
        return Term::Ptr{
            new OpaqueStringConstantTerm(
                TyF->getUnknownType(), v
            )
        };
    }

    Term::Ptr getOpaqueIndexingTerm(Term::Ptr lhv, Term::Ptr rhv) {
        return Term::Ptr{
            new OpaqueIndexingTerm(
                TyF->getUnknownType(),
                lhv,
                rhv
            )
        };
    }

    Term::Ptr getOpaqueMemberAccessTerm(Term::Ptr lhv, const std::string& property, bool indirect = false) {
        return Term::Ptr{
            new OpaqueMemberAccessTerm(
                TyF->getUnknownType(),
                lhv,
                property,
                indirect
            )
        };
    }

    Term::Ptr getOpaqueCallTerm(Term::Ptr lhv, const std::vector<Term::Ptr>& rhv) {
        return Term::Ptr{
            new OpaqueCallTerm(
                TyF->getUnknownType(),
                lhv,
                rhv
            )
        };
    }

    Term::Ptr getSignTerm(Term::Ptr rhv) {
        return Term::Ptr{
            new SignTerm(
                SignTerm::getTermType(TyF, rhv),
                rhv
            )
        };
    }

    Term::Ptr getAxiomTerm(Term::Ptr lhv, Term::Ptr rhv) {
        return Term::Ptr{
            new AxiomTerm(
                AxiomTerm::getTermType(TyF, lhv, rhv),
                lhv, rhv
            )
        };
    }

    Term::Ptr getBoundTerm(Term::Ptr rhv) {
        return Term::Ptr{
            new BoundTerm{
                TyF->getInteger(32, llvm::Signedness::Unsigned), // XXX: 32 -> ???
                rhv
            }
        };
    }

    Term::Ptr getCastTerm(llvm::CastType opc, Term::Ptr rhv) {
        return Term::Ptr{
            new CastTerm(
                opc, rhv
            )
        };
    }

    static TermFactory::Ptr get(
            SlotTracker* st,
            TypeFactory::Ptr TyF) {
        return TermFactory::Ptr{
            new TermFactory(st, TyF)
        };
    }

    static TermFactory::Ptr get(TypeFactory::Ptr TyF) {
        return TermFactory::Ptr{
            new TermFactory(nullptr, TyF)
        };
    }

private:

    SlotTracker* st;
    TypeFactory::Ptr TyF;

    TermFactory(SlotTracker* st, TypeFactory::Ptr TyF);

};

} /* namespace borealis */

#include "Util/unmacros.h"

#endif /* TERMFACTORY_H_ */
