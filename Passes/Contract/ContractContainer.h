//
// Created by kivi on 16.12.15.
//

#ifndef BOREALIS_CONTRACTSCONTAINER_H
#define BOREALIS_CONTRACTSCONTAINER_H

#include <vector>
#include <string>

#include "Contract.h"
#include "FunctionIdentifier.h"
#include "Protobuf/Gen/Passes/Contract/ContractContainer.pb.h"
#include "State/PredicateState.h"

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
    ContractContainer(const ContractContainer&) = default;
    ContractContainer(ContractContainer&&)      = default;
    ~ContractContainer()                        = default;

    ContractContainer(const std::vector<FunctionIdentifier::Ptr>& functions,
                      const std::vector<Contract::Ptr>& contracts) {
        for (auto i = 0U; i < functions.size(); ++i) {
            data_[functions[i]] = contracts[i];
        }
    }

    Contract::Ptr operator[](FunctionIdentifier::Ptr func) {
        if (auto&& op = util::at(data_, func)) {
            return op.getUnsafe();
        } else {
            data_[func] = Contract::Ptr{ new Contract({}) };
            return data_[func];
        }
    }

    Contract::Ptr at(FunctionIdentifier::Ptr func) {
        if (auto&& op = util::at(data_, func)) {
            return op.getUnsafe();
        } else {
            data_[func] = Contract::Ptr{ new Contract({}) };
            return data_[func];
        }
    }

    const Data& data() const {
        return data_;
    }

    size_t size() const {
        return data_.size();
    }

    FunctionIdentifier::Ptr getFunctionId(llvm::Function* F, std::pair<unsigned int, unsigned int> mem);

private:

    Data data_;

};

}   /* namespace borealis */

#endif //BOREALIS_CONTRACTSCONTAINER_H
