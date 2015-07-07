/*
 * ContractManager.h
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#ifndef CONTRACTMANAGER_H_
#define CONTRACTMANAGER_H_

#include <llvm/Pass.h>

#include <unordered_map>

#include "Term/Term.h"
#include "State/PredicateState.h"
#include "Factory/Nest.h"


namespace borealis {

class ContractManager : public llvm::ModulePass {

    using Args = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using ArgToTerm = std::unordered_map<int, Term::Ptr>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;

    using ContractStates = std::unordered_map<llvm::Function*, std::unordered_set<PredicateState::Ptr>>;
    using ContractArguments = std::unordered_map<llvm::Function*, ArgToTerm>;

public:

    static char ID;
    static ContractStates states;
    static ContractArguments arguments;

    ContractManager();
    virtual ~ContractManager() = default;

    virtual bool runOnModule(llvm::Module&) override;
    virtual void print(llvm::raw_ostream&, const llvm::Module*) const override;

    void addContract(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping);


private:

    PredicateState::Ptr killStateChoice(PredicateState::Ptr S, const FactoryNest& FN);
    PredicateState::Ptr unifyState(PredicateState::Ptr S, const FactoryNest& FN, llvm::Function* F, const TermMap& argumentsReplacement);
    PredicateState::Ptr mergeState(PredicateState::Ptr S, const FactoryNest& FN);
};

} /* namespace borealis */

#endif /* CONTRACTMANAGER_H_ */
