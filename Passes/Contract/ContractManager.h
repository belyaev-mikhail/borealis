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
#include "Passes/Manager/FunctionManager.h"


namespace borealis {

class ContractManager : public llvm::ModulePass {

    using Args = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using TermMap = std::unordered_map<Term::Ptr, Term::Ptr, TermHash, TermEquals>;
    using ContractStates = std::unordered_map<llvm::Function*, std::unordered_set<PredicateState::Ptr>>;
    using ContractArguments = std::unordered_map<llvm::Function*, Args>;
    using MemInfo = std::pair<unsigned int, unsigned int>;

public:

    static char ID;
    constexpr static double mergingConstant = 0.9;

    ContractManager();
    virtual ~ContractManager() = default;

    virtual bool runOnModule(llvm::Module&) override;
    virtual void print(llvm::raw_ostream&, const llvm::Module*) const override;

    void addContract(llvm::Function* F, const FactoryNest& FN, const FunctionManager& FM,
                     PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping);

    void addSummary(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S,
                    const std::unordered_map<int, Args>& mapping);

private:

    void saveState(llvm::Function* F, PredicateState::Ptr S);
    void getUniqueChoices(llvm::Function* F, std::vector<std::pair<PredicateState::Ptr, int>>& res) const;

    void printContracts() const;
    void printSummaries() const;

private:

    static ContractStates choiceContracts;
    static ContractStates basicContracts;
    static ContractArguments contractArguments;
    static std::unordered_map<llvm::Function*, unsigned int> functionCalls;
    static std::unordered_map<llvm::Function*, MemInfo> memBounds;

    static ContractStates summaries;
    static ContractArguments summaryArguments;

};

} /* namespace borealis */

#endif /* CONTRACTMANAGER_H_ */
