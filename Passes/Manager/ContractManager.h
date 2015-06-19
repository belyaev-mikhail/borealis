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

#include "Logging/logger.hpp"
#include "State/PredicateState.h"

namespace borealis {

class ContractManager : public llvm::ModulePass {

    using Args = std::unordered_set<Term::Ptr, TermHash, TermEquals>;
    using ArgToTerms = std::unordered_map<int, Args>;

    struct StateInfo {
        PredicateState::Ptr state;
        ArgToTerms mapping;
    };

    struct StateInfoHash {
        size_t operator()(StateInfo si) const noexcept {
            return util::hash::defaultHasher()(si.state);
        }
    };

    struct StateInfoEquals {
        bool operator()(StateInfo lhv, StateInfo rhv) const noexcept {
            return *lhv.state == *rhv.state;
        }
    };

    using ContractData = std::unordered_map<llvm::Function*, std::unordered_set<StateInfo, StateInfoHash, StateInfoEquals>>;

public:

    static char ID;
    static ContractData data;

    ContractManager();
    virtual ~ContractManager() = default;

    virtual bool runOnModule(llvm::Module&) override;
    virtual void print(llvm::raw_ostream&, const llvm::Module*) const;

    void addContract(llvm::Function* F, PredicateState::Ptr S, const ArgToTerms& mapping);

};

} /* namespace borealis */

#endif /* CONTRACTMANAGER_H_ */
