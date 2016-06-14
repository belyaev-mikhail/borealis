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
#include "Passes/Contract/Contract.h"

namespace borealis {

template<>
struct protobuf_traits<Contract> {
    typedef protobuf_traits<PredicateState> PredicateStateConverter;

    static std::unique_ptr<proto::Contract> toProtobuf(const Contract& a) {
        auto res = util::uniq(new proto::Contract());
        for (auto&& it : a.data()) {
            res->mutable_data()->AddAllocated(PredicateStateConverter::toProtobuf(*it).release());
        }
        return std::move(res);
    }

    static Contract::Ptr fromProtobuf(
            const FactoryNest& fn,
            const proto::Contract& t) {
        std::vector<PredicateState::Ptr> res;
        for (int i = 0; i < t.data().size(); ++i) {
            res.push_back(PredicateStateConverter::fromProtobuf(fn, t.data(i)));
        }
        return Contract::Ptr{ new Contract({ res }) };
    }
};

template<>
struct protobuf_traits<FunctionIdentifier> {

    static std::unique_ptr<proto::FunctionIdentifier> toProtobuf(const FunctionIdentifier& a) {
        auto res = util::uniq(new proto::FunctionIdentifier());
        res->set_functionname(a.name());
        res->set_rettype(a.rettype());
        res->set_calls(a.calls());
        res->set_memstart(a.memBounds().first);
        res->set_memend(a.memBounds().second);
        return std::move(res);
    }

    static FunctionIdentifier::Ptr fromProtobuf(const proto::FunctionIdentifier& t) {
        std::pair<unsigned int, unsigned int> mem = { t.memstart(), t.memend() };
        return FunctionIdentifier::Ptr{ new FunctionIdentifier(t.functionname(), t.rettype(), t.calls(), mem) };
    }
};

template<>
struct protobuf_traits<ContractContainer> {
    typedef protobuf_traits<Contract> ContractConverter;
    typedef protobuf_traits<FunctionIdentifier> FunctionConverter;

    static std::unique_ptr<proto::ContractContainer> toProtobuf(const ContractContainer& a) {
        auto res = util::uniq(new proto::ContractContainer());
        for (auto&& it : a.data()) {
            auto&& func = it.first;
            auto&& cont = it.second;
            res->mutable_function()->AddAllocated(FunctionConverter::toProtobuf(*func).release());
            res->mutable_data()->AddAllocated(ContractConverter::toProtobuf(*cont).release());
        }
        return std::move(res);
    }

    static ContractContainer::Ptr fromProtobuf(
            const FactoryNest& fn,
            const proto::ContractContainer& t) {
        std::vector<FunctionIdentifier::Ptr> func;
        std::vector<Contract::Ptr> cont;
        for (int i = 0; i < t.function_size(); ++i) {
            func.push_back(FunctionConverter::fromProtobuf(t.function(i)));
        }
        for (int i = 0; i < t.data_size(); ++i) {
            cont.push_back(ContractConverter::fromProtobuf(fn, t.data(i)));
        }
        return ContractContainer::Ptr{ new ContractContainer(func, cont) };
    }
};

}   /* namespace borealis */

#endif //BOREALIS_PROTOBUFCONTRACTIMPL_HPP
