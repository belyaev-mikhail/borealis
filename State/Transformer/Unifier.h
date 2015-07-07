//
// Created by kivi on 06.07.15.
//

#ifndef UNIFIER_H
#define UNIFIER_H

#include "Transformer.hpp"

namespace borealis {

class Unifier : public Transformer<Unifier> {

    using Base = Transformer<Unifier>;
    using TermSet = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;

public:

    Unifier(const FactoryNest& fn, const std::unordered_map<int, Term::Ptr>& a, const TermMap& m);

    Predicate::Ptr transformEqualityPredicate(EqualityPredicatePtr pred);
    Term::Ptr transformTerm(Term::Ptr term);
    Term::Ptr transformCmpTerm(CmpTermPtr term);

private:

    llvm::ConditionType invertCondition(llvm::ConditionType cond);
    Term::Ptr invertBoolean(Term::Ptr term);

    FactoryNest FN;
    bool isInverted;
    TermSet args;
    TermMap termMap;

};

}   /* namespace borealis */

#endif //UNIFIER_H
