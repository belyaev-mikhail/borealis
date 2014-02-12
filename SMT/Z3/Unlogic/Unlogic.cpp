/*
 * Unlogic.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: sam
 */

#include "Factory/Nest.h"
#include "SMT/Z3/Unlogic/Unlogic.h"

#include "Util/macros.h"

namespace borealis {
namespace z3_ {
namespace unlogic {

USING_SMT_LOGIC(Z3);

Term::Ptr undoBv(const z3::expr& expr, const FactoryNest& FN) {
    long long i;
    auto res = Z3_get_numeral_int64(expr.ctx(), expr, &i);
    ASSERT(res != 0, "Something bad occurs while getting int value from Z3 expression");
    return FN.Term->getIntTerm(i, llvm::Signedness::Signed);
}

Term::Ptr undoBool(const z3::expr& expr, const FactoryNest& FN) {
    auto res = Z3_get_bool_value(expr.ctx(), expr);
    if (res == Z3_L_TRUE) {
        return FN.Term->getTrueTerm();
    } else if (res == Z3_L_FALSE) {
        return FN.Term->getFalseTerm();
    } else {
        BYE_BYE(Term::Ptr, "Trying to unbool");
    }
}

Term::Ptr undoThat(Z3::Dynamic dyn) {
    FactoryNest FN(nullptr);

    auto expr = logic::z3impl::getExpr(dyn);
    ASSERT(expr.is_numeral() || expr.is_bool(), "Trying to unlogic non-numeral or bool.");

    if (expr.is_bv()) {
        return undoBv(expr, FN);
    } else if (expr.is_bool()) {
        return undoBool(expr, FN);
    }

    BYE_BYE(Term::Ptr, "Unsupported numeral type.");
}

} // namespace unlogic
} // namespace z3_
} // namespace borealis

#include "Util/unmacros.h"