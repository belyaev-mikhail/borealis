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

#include "ContractContainer.h"
#include "Factory/Nest.h"
#include "Passes/Manager/FunctionManager.h"
#include "State/PredicateState.h"
#include "Term/Term.h"


namespace borealis {

class ContractManager : public llvm::ModulePass {

    using Args = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using FunctionInfo = std::unordered_map<FunctionIdentifier::Ptr, std::pair<llvm::Function*, FunctionManager*>,
                                            FunctionIdHash, FunctionIdEquals>;
    using MemInfo = std::pair<unsigned int, unsigned int>;

public:

    struct Summary{
        llvm::Function* func;
        PredicateStateImply::Ptr state;
        Summary(llvm::Function* F, PredicateStateImply::Ptr S): func(F), state(S) {}
    };

    static char ID;
    //magic numbers
    constexpr static double MERGING_CONSTANT = 0.68;
    constexpr static int MIN_FUNCTION_CALLS = 3;
    const std::string PROTOBUF_FILE = "contracts.out";

    ContractManager();
    virtual ~ContractManager()  = default;

    virtual bool runOnModule(llvm::Module&) override;
    virtual void print(llvm::raw_ostream&, const llvm::Module*) const override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;
    virtual bool doFinalization(llvm::Module &) override;

    void addContract(llvm::Function* F, FunctionManager& FM, PredicateState::Ptr S,
                     const std::unordered_map<int, Args>& mapping);

    void addSummary(llvm::Function* F, PredicateStateImply::Ptr S, FunctionManager& FM);


private:

    void saveState(FunctionIdentifier::Ptr func, PredicateState::Ptr state);
    Term::Ptr stateToTerm(PredicateState::Ptr state) const;

    void printContracts() const;
    void printSummaries() const;

    ContractContainer::Ptr readFromDB();
    ContractContainer::Ptr readFromFile(const std::string &fname);

    void writeToDB() const;
    void writeToFile(const std::string &fname) const;


private:

    static ContractContainer::Ptr contracts;
    static FunctionInfo visitedFunctions;
    static std::vector<Summary> summaries;

    FactoryNest FN;

};

} /* namespace borealis */

#endif /* CONTRACTMANAGER_H_ */
