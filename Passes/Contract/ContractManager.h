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
#include "ContractContainer.h"


namespace borealis {

class ContractManager : public llvm::ModulePass {

    using Args = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using FunctionSet = std::unordered_set<FunctionIdentifier::Ptr, FunctionIdHash, FunctionIdEquals>;
    using ContractStates = std::unordered_map<llvm::Function*, std::unordered_set<PredicateState::Ptr>>;
    using ContractArguments = std::unordered_map<llvm::Function*, Args>;
    using MemInfo = std::pair<unsigned int, unsigned int>;

public:

    static char ID;
    constexpr static double mergingConstant = 0.9;  //magic number
    const std::string PROTOBUF_FILE = "contracts.out";

    ContractManager();
    virtual ~ContractManager()  = default;

    virtual bool runOnModule(llvm::Module&) override;
    virtual void print(llvm::raw_ostream&, const llvm::Module*) const override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;
    virtual bool doFinalization(llvm::Module &) override;

    void addContract(llvm::Function* F, const FunctionManager& FM, PredicateState::Ptr S,
                     const std::unordered_map<int, Args>& mapping);

    void addSummary(llvm::Function* F, PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping);

private:

    void saveState(FunctionIdentifier::Ptr func, PredicateState::Ptr state);
    Term::Ptr stateToTerm(PredicateState::Ptr state);

    void printContracts() const;
    void printSummaries() const;

    ContractContainer::Ptr readFrom(const std::string& fname);
    void writeTo(const std::string& fname) const;


private:

    static ContractContainer::Ptr contracts;
    static FunctionSet visitedFunctions;
    static ContractStates summaries;

    FactoryNest FN;

};

} /* namespace borealis */

#endif /* CONTRACTMANAGER_H_ */
