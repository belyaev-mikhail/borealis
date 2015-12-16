//
// Created by danya on 26.11.15.
//

#ifndef BOREALIS_UNNECESPREDDELETER_H
#define BOREALIS_UNNECESPREDDELETER_H


#include <unordered_set>
#include <vector>

#include "State/Transformer/Transformer.hpp"

namespace borealis {

    class UnnecesPredDeleter : public borealis::Transformer<UnnecesPredDeleter> {

        using Base = Transformer<UnnecesPredDeleter>;

        using ChoiceInfo = std::vector<std::vector<Predicate::Ptr>>;
        using PrVector = std::vector<Predicate::Ptr>;
        using StVector = std::vector<PredicateState::Ptr>;
        using TermSet = std::unordered_set<Term::Ptr,TermHash, TermEquals>;

    public:
        UnnecesPredDeleter(const FactoryNest& FN,const PrVector& prPred,const TermSet& suspT,
                           llvm::iplist<llvm::Argument>& ar);

        const TermSet getRigthTerms() const{
            return contractTerms;
        }

        using Base::transform;

        PredicateState::Ptr transform(PredicateState::Ptr ps);
        PredicateState::Ptr transformChoice(PredicateStateChoicePtr pred);


        using Base::transformBase;

    private:

        TermSet intersTerms;
        TermSet contractTerms;
        TermSet mayInfl;
        PrVector protPred;
        TermSet suspTerms;
        llvm::iplist<llvm::Argument>& args;
    };

} /*namespace borealis*/
#endif //BOREALIS_UNNECESPREDDELETER_H
