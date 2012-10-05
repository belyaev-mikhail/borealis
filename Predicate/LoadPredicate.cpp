/*
 * LoadPredicate.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#include "LoadPredicate.h"

namespace borealis {

LoadPredicate::LoadPredicate(
		const llvm::Value* lhv,
		const llvm::Value* rhv,
		SlotTracker* st) :
				lhv(lhv),
				rhv(rhv),
				lhvs(st->getLocalName(lhv)),
				rhvs(st->getLocalName(rhv)),
				asString(lhvs + "=*" + rhvs) {
}

std::string LoadPredicate::toString() const {
	return asString;
}

Predicate::Key LoadPredicate::getKey() const {
	return std::make_pair(std::type_index(typeid(*this)).hash_code(), lhv);
}

} /* namespace borealis */
