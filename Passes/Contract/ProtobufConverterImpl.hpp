//
// Created by kivi on 17.12.15.
//

#ifndef BOREALIS_PROTOBUFCONTRACTIMPL_HPP
#define BOREALIS_PROTOBUFCONTRACTIMPL_HPP


#include "Protobuf/Gen/Passes/Contract/ContractContainer.pb.h"
#include "Protobuf/Gen/Passes/Contract/FunctionIdentifier.pb.h"
#include "State/ProtobufConverterImpl.hpp"

#include "Passes/Contract/ContractContainer.h"
#include "Passes/Contract/FunctionIdentifier.h"

namespace borealis {

template<>
struct protobuf_traits<FunctionIdentifier> {

    static std::unique_ptr<proto::FunctionIdentifier> toProtobuf(const FunctionIdentifier& a) {
        auto res = util::uniq(new proto::FunctionIdentifier());
        res->set_functionname(a.name());
        res->set_rettype(a.rettype());
        res->set_calls(a.calls());
        return std::move(res);
    }

    static FunctionIdentifier::Ptr fromProtobuf(const proto::FunctionIdentifier& t) {
        return FunctionIdentifier::Ptr{ new FunctionIdentifier(t.functionname(), t.rettype(), t.calls()) };
    }
};

template<>
struct protobuf_traits<ContractContainer> {
    typedef protobuf_traits<PredicateState> PredicateStateConverter;
    typedef protobuf_traits<FunctionIdentifier> FunctionConverter;

    static std::unique_ptr<proto::ContractContainer> toProtobuf(const ContractContainer& a) {
        auto res = util::uniq(new proto::ContractContainer());
        res->set_allocated_function(FunctionConverter::toProtobuf(*a.function()).release());
        for (auto&& state : a.data()) {
            res->mutable_data()->AddAllocated(PredicateStateConverter::toProtobuf(*state).release());
        }
        return std::move(res);
    }

    static ContractContainer::Ptr fromProtobuf(
            const FactoryNest& fn,
            const proto::ContractContainer& t) {
        std::vector<PredicateState::Ptr> ret;
        for (int i = 0; i < t.data().size(); ++i) {
            ret.push_back(PredicateStateConverter::fromProtobuf(fn, t.data(i)));
        }
        auto&& function = FunctionConverter::fromProtobuf(t.function());
        return ContractContainer::Ptr{ new ContractContainer(function, ret) };
    }
};

}   /* namespace borealis */

#endif //BOREALIS_PROTOBUFCONTRACTIMPL_HPP
