/*
 * UnusedVariableDeleter.h
 *
 *  Created on: Mar 30, 2016
 *      Author: stepanov
 */

#ifndef STATE_TRANSFORMER_UNUSEDVARIABLEDELETER_H_
#define STATE_TRANSFORMER_UNUSEDVARIABLEDELETER_H_

#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/AliasSetTracker.h>

#include <unordered_set>

#include "State/PredicateState.def"
#include "State/Transformer/CachingTransformer.hpp"
#include "State/Transformer/LocalStensgaardAA.h"
#include "State/Transformer/TermCollector.h"

namespace borealis {


class UnusedVariablesDeleter : public borealis::CachingTransformer<UnusedVariablesDeleter> {

    using Base = borealis::CachingTransformer<UnusedVariablesDeleter>;
    using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;

public:
    UnusedVariablesDeleter(FactoryNest FN, const TermSet& ts);

    template<class T>
    UnusedVariablesDeleter(FactoryNest FN, const T& obj): Base(FN) {
        auto&& tc = TermCollector(FN);
        tc.transform(obj);
        usedVars=tc.getTerms();
    }

    using Base::transform;

    PredicateState::Ptr transform(PredicateState::Ptr ps);

    using Base::transformBase;

    Predicate::Ptr transformBase(Predicate::Ptr pred);

private:
    TermSet usedVars;
};

} /* namespace borealis */

#endif /* STATE_TRANSFORMER_UNUSEDVARIABLEDELETER_H_ */