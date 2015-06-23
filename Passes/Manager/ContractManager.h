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

#include "../../Term/Term.h"
#include "../../State/PredicateState.h"
#include "../../Factory/Nest.h"

namespace borealis {

class ContractManager : public llvm::ModulePass {

    using Args = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using ArgToTerms = std::unordered_map<int, Args>;

    using ContractStates = std::unordered_map<llvm::Function*, std::unordered_set<PredicateState::Ptr>>;
    using ContractArguments = std::unordered_map<llvm::Function*, ArgToTerms>;

public:

    static char ID;
    static ContractStates states;
    static ContractArguments arguments;

    ContractManager();
    virtual ~ContractManager() = default;

    virtual bool runOnModule(llvm::Module&) override;
    virtual void print(llvm::raw_ostream&, const llvm::Module*) const override;

    void addContract(llvm::Function* F, PredicateState::Ptr S, const ArgToTerms& mapping);


private:

    void replaseTerms();

};

} /* namespace borealis */

#endif /* CONTRACTMANAGER_H_ */
