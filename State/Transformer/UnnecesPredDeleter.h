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
       // using VecTermSet =  std::vector<std::unordered_set<Term::Ptr,TermHash, TermEquals>>;
        using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;

    public:
        UnnecesPredDeleter(const FactoryNest& FN,const PrVector& prPred,const TermSet& suspT,
                           llvm::iplist<llvm::Argument>& ar);

        const TermSet getRightTerms() const{
            return contractTerms;
        }

        const StVector getResultVec() const{
            return resultVec;
        }

        using Base::transform;

        PredicateState::Ptr transform(PredicateState::Ptr ps);
        BasicPredicateState::Ptr transformBasic(BasicPredicateStatePtr pred);
        BasicPredicateState::Ptr transformBasicPredicateState(BasicPredicateStatePtr pred);
        PredicateState::Ptr transformPredicateState(PredicateState::Ptr state);
        Predicate::Ptr transformBase(Predicate::Ptr pr);

        using Base::transformBase;

        bool isOpaqueTerm(Term::Ptr term);
    private:

        TermSet intersTerms;
        TermSet contractTerms;
        TermSet temp;
        TermSet mayInfl;
        PrVector protPred;
        TermSet suspTerms;
        StVector resultVec;
        StVector newChoice;
        llvm::iplist<llvm::Argument>& args;
        TermMap chMap;
        TermSet argTerms;
        bool isDel;
        bool inResVect;
    };

} /*namespace borealis*/
#endif //BOREALIS_UNNECESPREDDELETER_H
