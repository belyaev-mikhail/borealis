/*
 * StorePredicate.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#include "StorePredicate.h"

namespace borealis {

StorePredicate::StorePredicate(
		const llvm::Value* lhv,
		const llvm::Value* rhv,
		SlotTracker* st) :
				lhv(lhv),
				rhv(rhv),
				lhvs("*" + st->getLocalName(lhv)),
				rhvs(st->getLocalName(rhv)) {
	this->asString = lhvs + "=" + rhvs;
}

Predicate::Key StorePredicate::getKey() const {
	return std::make_pair(std::type_index(typeid(*this)).hash_code(), lhv);
}

z3::expr StorePredicate::toZ3(z3::context& ctx) const {
	using namespace::z3;

	expr l = derefValueToExpr(ctx, *lhv, lhvs);
	expr r = valueToExpr(ctx, *rhv, rhvs);
	return l == r;
}

} /* namespace borealis */
