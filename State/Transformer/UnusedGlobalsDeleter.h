//
// Created by stepanov on 2/29/16.
//

#ifndef BOREALIS_UNUSEDGLOBALSDELETER_H
#define BOREALIS_UNUSEDGLOBALSDELETER_H


#include <llvm/IR/Instructions.h>

#include "Transformer.hpp"

namespace borealis {

    class UnusedGlobalsDeleter : public Transformer<UnusedGlobalsDeleter> {

        using Base = Transformer<UnusedGlobalsDeleter>;
        using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;

    public:

        using Base::transform;

        PredicateState::Ptr transform(PredicateState::Ptr ps);
        UnusedGlobalsDeleter(const FactoryNest& fn, const TermSet pr);
        Predicate::Ptr transformPredicate(Predicate::Ptr pred);
        void filterTerms(TermSet& ts);

    private:

        TermSet protTerm;
    };


}



#endif //BOREALIS_UNUSEDGLOBALSDELETER_H
