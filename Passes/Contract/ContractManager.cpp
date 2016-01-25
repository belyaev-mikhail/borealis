/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <Protobuf/Converter.hpp>
#include <fstream>
#include <Util/passes.hpp>
#include "State/Transformer/Retyper.h"
#include "State/Transformer/Unifier.h"
#include "State/Transformer/StateOptimizer.h"
#include "State/Transformer/MergingTransformer.h"
#include "State/Transformer/ChoiceKiller.h"

#include "ContractManager.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module&) {
    FactoryNest FN = FactoryNest(nullptr);
    VariableInfoTracker* VI = &GetAnalysis<VariableInfoTracker>::doit(this);
    FN.Type->initialize(*VI);

    contracts = readFrom(protoFile);
    if (not contracts) {
        contracts = ContractContainer::Ptr{new ContractContainer()};
    }
    return false;
}

void ContractManager::addContract(llvm::Function* F, const FactoryNest& FN, const FunctionManager& FM,
                                  PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping) {
    auto&& func = contracts->getFunctionId(F, FM.getMemoryBounds(F));
    func->called();
    if (not S->isEmpty()) {
        auto&& unifier = Unifier(FN, mapping);
        auto&& unified = unifier.transform(S);
        auto&& retyped = Retyper(FN).transform(unified);
        auto&& choiceKilled = ChoiceKiller(FN, func->memBounds()).transform(retyped);
        if (not choiceKilled->isEmpty()) {
            auto&& optimized = StateOptimizer(FN).transform(choiceKilled);
            contracts->at(func)->push_back(optimized);
            writeTo(protoFile);
        }
    }
}

void ContractManager::addSummary(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S,
                                  const std::unordered_map<int, Args>& mapping) {
    if (not S->isEmpty()) {
        auto&& unifier = Unifier(FN, mapping);
        auto&& unified = unifier.transform(S);
        auto&& optimized = StateOptimizer(FN).transform(unified);
        if (not optimized->isEmpty()) {
            summaries[F].insert(optimized);
        }
    }
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {
    printContracts();
    printSummaries();
}

void ContractManager::printContracts() const {
    ContractStates result;

    //printing results
    auto&& dbg = dbgs();
    for (auto&& it : contracts->data()) {
        errs() << it.first->name() << endl;
        for (auto&& st : *it.second) {
            errs() << st << endl;
        }
    }

    dbg << "Contract extraction results" << endl;
    for (auto&& it : result) {
        auto&& F = it.first;
        dbg << "---" << "Function " << F->getName() << "---" << endl;

        for(auto&& state : it.second) {
            dbg << "State:" << endl;
            dbg << state << endl;
        }

        dbg << endl;
    }
    dbg << end;
}

void ContractManager::printSummaries() const {
    auto&& dbg = dbgs();

    dbg << "Summary extraction results" << endl;
    for (auto&& it : summaries) {
        dbg << "---" << "Function " << it.first->getName() << "---" << endl;

        for (auto&& state : it.second) {
            dbg << "State:" << endl;
            dbg << state << endl;
        }

        dbg << endl;
    }
    dbg << end;
}

ContractContainer::Ptr ContractManager::readFrom(const std::string& fname) {
    std::ifstream contractsStream(fname, std::iostream::in | std::iostream::binary);
    if (not contractsStream) {
        return nullptr;
    }

    ContractContainer::ProtoPtr proto{new proto::ContractContainer{}};
    proto->ParseFromIstream(&contractsStream);

    ContractContainer::Ptr container = deprotobuffy(FactoryNest(), *proto);
    if (not container) {
        return nullptr;
    }

    return container;
}

void ContractManager::writeTo(const std::string& fname) const {
    ContractContainer::ProtoPtr proto = protobuffy(contracts);
    std::ofstream contractsStream(fname, std::iostream::out | std::iostream::binary);
    proto->SerializeToOstream(&contractsStream);
}

void ContractManager::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
    Info.setPreservesAll();

    AUX<VariableInfoTracker>::addRequiredTransitive(Info);
}

char ContractManager::ID = 0;
ContractContainer::Ptr ContractManager::contracts;

ContractManager::ContractStates ContractManager::summaries;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
