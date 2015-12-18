//
// Created by kivi on 16.12.15.
//

#ifndef BOREALIS_CONTRACTSCONTAINER_H
#define BOREALIS_CONTRACTSCONTAINER_H

#include <vector>
#include <string>

#include "State/PredicateState.h"
#include "Protobuf/Gen/Passes/Contract/ContractContainer.pb.h"
#include "FunctionIdentifier.h"

namespace borealis {

/** protobuf -> Passes/Contract/ContractContainer.proto

import "State/PredicateState.proto";
import "Passes/Contract/FunctionIdentifier.proto";

package borealis.proto;

message ContractContainer {
    optional borealis.proto.FunctionIdentifier function = 1;
    repeated borealis.proto.PredicateState data = 2;
}

**/

class ContractContainer {

    using Contracts = std::vector<PredicateState::Ptr>;

public:

    using Ptr = std::shared_ptr<ContractContainer>;
    using ProtoPtr = std::unique_ptr<borealis::proto::ContractContainer>;

    ContractContainer(const FunctionIdentifier::Ptr& function,
                      const std::vector<PredicateState::Ptr>& st) :
            function_(function), data_(st) {}

    ContractContainer(llvm::Function* F, const unsigned int calls) {
        function_ = FunctionIdentifier::Ptr{ new FunctionIdentifier(F, calls) };
    }

    ContractContainer(const ContractContainer&) = default;
    ContractContainer(ContractContainer&&)      = default;
    ~ContractContainer()                        = default;

    void addContract(const PredicateState::Ptr& ptr) {
        data_.push_back(ptr);
    }

    void addContract(PredicateState::Ptr& ptr) {
        data_.push_back(ptr);
    }

    const Contracts& data() const {
        return data_;
    }

    FunctionIdentifier::Ptr function() const {
        return function_;
    }

private:

    FunctionIdentifier::Ptr function_;
    Contracts data_;

};

}   /* namespace borealis */

#endif //BOREALIS_CONTRACTSCONTAINER_H
