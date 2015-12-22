//
// Created by kivi on 18.12.15.
//

#ifndef BOREALIS_CONTRACTS_H
#define BOREALIS_CONTRACTS_H

#include <vector>

#include "State/PredicateState.h"
#include "Protobuf/Gen/Passes/Contract/Contract.pb.h"

#include "Util/macros.h"

namespace borealis {

/** protobuf -> Passes/Contract/Contract.proto

import "State/PredicateState.proto";

package borealis.proto;

message Contract {
    repeated borealis.proto.PredicateState data = 1;
}

**/


class Contract {

    using Data = std::vector<PredicateState::Ptr>;
    Data data_;

public:

    using Ptr = std::shared_ptr<Contract>;
    using ProtoPtr = std::unique_ptr<borealis::proto::Contract>;

    Contract(const Data& data) : data_(data) {}
    Contract(const Contract&) = default;
    Contract(Contract&&)      = default;
    ~Contract()               = default;

    auto begin() QUICK_RETURN(data_.begin());
    auto begin() QUICK_CONST_RETURN(data_.begin());
    auto end() QUICK_RETURN(data_.end());
    auto end() QUICK_CONST_RETURN(data_.begin());

    const PredicateState& operator[](size_t ix)       { return *data_[ix]; }
    const PredicateState& operator[](size_t ix) const { return *data_.at(ix); }

    void push_back(const PredicateState::Ptr& ptr) {
        data_.push_back(ptr);
    }

    void push_back(PredicateState::Ptr&& ptr) {
        data_.push_back(std::move(ptr));
    }

    const Data& data() const {
        return data_;
    }

};

}   /* namespace borealis */

#include "Util/unmacros.h"

#endif //BOREALIS_CONTRACTS_H
