//
// Created by kivi on 06.07.15.
//

#ifndef UNIFIER_H
#define UNIFIER_H

#include "Transformer.hpp"

namespace borealis {

class ArgumentUnifier : public Transformer<ArgumentUnifier> {

    using Base = Transformer<ArgumentUnifier>;
    using TermSet = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;
    using ArgToTerms = std::unordered_map<int, TermSet>;

public:

    const std::string ARGUMENT_PREFIX = "arg$";

    ArgumentUnifier(const FactoryNest& fn, const ArgToTerms& a);

    Term::Ptr transformTerm(Term::Ptr term);
    const TermSet& getArguments();

private:

    bool isSigned(llvm::ConditionType cond);

private:

    FactoryNest FN;
    TermSet args;
    ArgToTerms argsMapping;

};

}   /* namespace borealis */

#endif //UNIFIER_H
