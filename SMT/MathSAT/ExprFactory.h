/*
 * ExprFactory.h
 *
 *  Created on: Aug 5, 2013
 *      Author: sam
 */

#ifndef MATHSAT_EXPRFACTORY_H_
#define MATHSAT_EXPRFACTORY_H_

#include <llvm/Target/TargetData.h>

#include "SMT/MathSAT/MathSAT.h"
#include "SMT/MathSAT/MathSatTypes.h"
#include "Util/util.h"

namespace borealis {
namespace mathsat_ {

class ExprFactory {

    USING_SMT_LOGIC(MathSAT);

public:

#include "Util/macros.h"
    static size_t sizeForType(Type::Ptr type) {
        using llvm::isa;
        using llvm::dyn_cast;
        if (auto integer = dyn_cast<type::Integer>(type))
            if (integer->getBitsize() > Integer::bitsize)
                return Long::bitsize;
            else
                return Integer::bitsize;
        else if (isa<type::Pointer>(type))
            return Pointer::bitsize;
        else if (isa<type::Array>(type))
            return Pointer::bitsize; // FIXME: ???
        else if (isa<type::Float>(type))
            return Real::bitsize;
        BYE_BYE(size_t,  "Cannot acquire bitsize for type "
                          + util::toString(*type));
    }
#include "Util/unmacros.h"

    ExprFactory();
    ExprFactory(const ExprFactory&) = delete;
    ExprFactory(ExprFactory&&) = delete;

    mathsat::Env& unwrap() {
        return *env;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Pointers
    Pointer getPtrVar(const std::string& name, bool fresh = false);
    Pointer getPtrConst(int ptr);
    Pointer getNullPtr();
    // Bools
    Bool getBoolVar(const std::string& name, bool fresh = false);
    Bool getBoolConst(bool b);
    Bool getTrue();
    Bool getFalse();
    // Integers
    Integer getIntVar(const std::string& name, bool fresh = false);
    Integer getIntConst(int v);
    Long getLongVar(const std::string& name, bool fresh = false);
    Long getLongConst(long long v);
    // Reals
    Real getRealVar(const std::string& name, bool fresh = false);
    Real getRealConst(int v);
    Real getRealConst(double v);
    // Memory
    MemArray getNoMemoryArray(const std::string& id);

    // Generic functions
    Dynamic getVarByTypeAndName(
            Type::Ptr type,
            const std::string& name,
            bool fresh = false);

    // Valid/invalid pointers
    Pointer getInvalidPtr();
    Bool isInvalidPtrExpr(Pointer ptr);
    // Misc pointer stuff
    Bool getDistinct(const std::vector<Pointer>& exprs);

#include "Util/macros.h"
    auto if_(Bool cond) QUICK_RETURN(logic::if_(cond))
#include "Util/unmacros.h"

    template<class T, class U>
    T switch_(
            U val,
            const std::vector<std::pair<U, T>>& cases,
            T default_) {
        return logic::switch_(val, cases, default_);
    }

    template<class T>
    T switch_(
            const std::vector<std::pair<Bool, T>>& cases,
            T default_) {
        return logic::switch_(cases, default_);
    }

    static void initialize(llvm::TargetData* TD);

private:

    std::unique_ptr<mathsat::Env> env;

    static unsigned int pointerSize;

};

} // namespace mathsat_
} // namespace borealis

#endif /* MATHSAT_EXPRFACTORY_H_ */
