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
#include "Contract.h"

namespace borealis {

/** protobuf -> Passes/Contract/ContractContainer.proto

import "Passes/Contract/FunctionIdentifier.proto";
import "Passes/Contract/Contract.proto";

package borealis.proto;

message ContractContainer {
    repeated borealis.proto.FunctionIdentifier function = 1;
    repeated borealis.proto.Contract data = 2;
}

**/

class ContractContainer {

    using Data = std::unordered_map<FunctionIdentifier::Ptr, Contract::Ptr, FunctionIdHash, FunctionIdEquals>;

public:

    using Ptr = std::shared_ptr<ContractContainer>;
    using ProtoPtr = std::unique_ptr<borealis::proto::ContractContainer>;

    ContractContainer()                         = default;

    ContractContainer(const std::vector<FunctionIdentifier::Ptr>& functions,
                      const std::vector<Contract::Ptr>& contracts) {
        for (auto i = 0U; i < functions.size(); ++i) {
            data_[functions[i]] = contracts[i];
        }
    }

    ContractContainer(const ContractContainer&) = default;
    ContractContainer(ContractContainer&&)      = default;
    ~ContractContainer()                        = default;

    Contract::Ptr operator[](FunctionIdentifier::Ptr func) {
        return data_[func];
    }

    const Data& data() const {
        return data_;
    }

private:

    Data data_;

};

}   /* namespace borealis */

#endif //BOREALIS_CONTRACTSCONTAINER_H
