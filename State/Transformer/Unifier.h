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
    using ArgToTerms = std::unordered_map<int, TermSet>;

public:

    const std::string ARGUMENT_PREFIX = "arg$";

    Unifier(const FactoryNest& fn, const ArgToTerms& a);

    Predicate::Ptr transformEqualityPredicate(EqualityPredicatePtr pred);
    Term::Ptr transformTerm(Term::Ptr term);
    Term::Ptr transformCmpTerm(CmpTermPtr term);
    Term::Ptr transformBinaryTerm(BinaryTermPtr term);
    const TermSet& getArguments();

private:

    llvm::ConditionType invertCondition(llvm::ConditionType cond);
    Term::Ptr invertBoolean(Term::Ptr term);
    Term::Ptr revertCmpTerm(CmpTermPtr term);
    Predicate::Ptr revertEqualityPredicate(EqualityPredicatePtr pred);
    bool containArgs(Term::Ptr term);
    bool isSigned(llvm::ConditionType cond);

private:

    FactoryNest FN;
    TermSet args;
    ArgToTerms argsMapping;

};

}   /* namespace borealis */

#endif //UNIFIER_H
