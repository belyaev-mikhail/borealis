//
// Created by kivi on 16.12.15.
//

#ifndef BOREALIS_CONTRACTSCONTAINER_H
#define BOREALIS_CONTRACTSCONTAINER_H

#include <vector>
#include <string>

#include "State/PredicateState.h"
#include "Protobuf/Gen/Passes/Contract/ContractContainer.pb.h"

#include "Util/macros.h"

namespace borealis {

/** protobuf -> Passes/Contract/ContractContainer.proto

import "State/PredicateState.proto";

package borealis.proto;

message ContractContainer {
    optional string FunctionName = 1;
    optional string RetType = 2;
    repeated borealis.proto.PredicateState states = 3;
}

**/

class ContractContainer {

    using Contracts = std::vector<PredicateState::Ptr>;

public:

    using Ptr = std::shared_ptr<ContractContainer>;
    using ProtoPtr = std::unique_ptr<borealis::proto::ContractContainer>;

    ContractContainer(const std::string& fname,
                      const std::string& rettype,
                      const std::vector<PredicateState::Ptr>& st) :
            functionName_(fname), retType_(rettype), data_(st) {}

    ContractContainer(const ContractContainer&) = default;
    ContractContainer(ContractContainer&&)      = default;

    void addContract(const PredicateState::Ptr& ptr) {
        data_.push_back(ptr);
    }

    void addContract(PredicateState::Ptr& ptr) {
        data_.push_back(ptr);
    }

    const Contracts& data() const {
        return data_;
    }

    void setFunction(llvm::Function* F) {
        functionName_ = F->getName();
        llvm::raw_string_ostream rso(retType_);
        F->getReturnType()->print(rso);
    }

    const std::string& getFunctionName() const {
        return functionName_;
    }

    const std::string& getRetType() const {
        return retType_;
    }

private:

    std::string functionName_;
    std::string retType_;
    Contracts data_;

};

}   /* namespace borealis */

#include "Util/unmacros.h"

#endif //BOREALIS_CONTRACTSCONTAINER_H
