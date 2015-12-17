//
// Created by kivi on 17.12.15.
//

#ifndef BOREALIS_PROTOBUFCONTRACTIMPL_HPP
#define BOREALIS_PROTOBUFCONTRACTIMPL_HPP


#include "Protobuf/Gen/Passes/Contract/ContractContainer.pb.h"
#include "State/ProtobufConverterImpl.hpp"

#include "Passes/Contract/ContractContainer.h"

namespace borealis {

template<>
struct protobuf_traits<ContractContainer> {
    typedef protobuf_traits<PredicateState> PredicateStateConverter;

    static std::unique_ptr<proto::ContractContainer> toProtobuf(const ContractContainer& a) {
        auto res = util::uniq(new proto::ContractContainer());
        res->set_functionname(a.getFunctionName());
        res->set_rettype(a.getRetType());
        for (auto&& state : a.data()) {
            res->mutable_states()->AddAllocated(PredicateStateConverter::toProtobuf(*state).release());
        }
        return std::move(res);
    }

    static ContractContainer::Ptr fromProtobuf(
            const FactoryNest& fn,
            const proto::ContractContainer& t) {
        std::vector<PredicateState::Ptr> ret;
        for (int i = 0; i < t.states().size(); ++i) {
            ret.push_back(PredicateStateConverter::fromProtobuf(fn, t.states(i)));
        }
        return ContractContainer::Ptr{ new ContractContainer(t.functionname(), t.rettype(), ret) };
    }
};

}   /* namespace borealis */

#endif //BOREALIS_PROTOBUFCONTRACTIMPL_HPP
