/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <fstream>

#include "Protobuf/Converter.hpp"
#include "Util/passes.hpp"
#include "State/Transformer/Retyper.h"
#include "State/Transformer/ArgumentUnifier.h"
#include "State/Transformer/StateOptimizer.h"
#include "State/Transformer/MergingTransformer.h"
#include "State/Transformer/ChoiceKiller.h"
#include "ContractManager.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::doFinalization(llvm::Module&) {
    writeTo(PROTOBUF_FILE);
    return false;
}

bool ContractManager::runOnModule(llvm::Module& M) {
    DL = M.getDataLayout();
    FactoryNest FN = FactoryNest(DL, nullptr);
    VariableInfoTracker* VI = &GetAnalysis<VariableInfoTracker>::doit(this);
    FN.Type->initialize(*VI);

    contracts = readFrom(PROTOBUF_FILE);
    if (not contracts) {
        contracts = ContractContainer::Ptr{new ContractContainer()};
    }

    return false;
}

void ContractManager::addContract(llvm::Function* F, const FactoryNest& FN, const FunctionManager& FM,
                                  PredicateState::Ptr S, const std::unordered_map<int, Args>& mapping) {
    auto&& func = contracts->getFunctionId(F, FM.getMemoryBounds(F));
    func->called();
    visitedFunctions.insert(func);
    if (not S->isEmpty()) {
        auto&& unifier = ArgumentUnifier(FN, mapping);
        auto&& unified = unifier.transform(S);
        auto&& retyped = Retyper(FN).transform(unified);
        auto&& choiceKilled = ChoiceKiller(FN, func->memBounds()).transform(retyped);
        if (not choiceKilled->isEmpty()) {
            auto&& optimized = StateOptimizer(FN).transform(choiceKilled);
            saveState(func, optimized);
        }
    }
}

void ContractManager::addSummary(llvm::Function* F, const FactoryNest& FN, PredicateState::Ptr S,
                                  const std::unordered_map<int, Args>& mapping) {
    if (not S->isEmpty()) {
        auto&& unifier = ArgumentUnifier(FN, mapping);
        auto&& unified = unifier.transform(S);
        auto&& optimized = StateOptimizer(FN).transform(unified);
        if (not optimized->isEmpty()) {
            summaries[F].insert(optimized);
        }
    }
}

void ContractManager::saveState(FunctionIdentifier::Ptr func, PredicateState::Ptr state) {
    if (auto&& chain = llvm::dyn_cast<PredicateStateChain>(state)) {
        saveState(func, chain->getBase());
        saveState(func, chain->getCurr());
    } else {
        contracts->at(func)->push_back(state);
    }
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {
    printContracts();
    printSummaries();
}

void ContractManager::printContracts() const {
    auto&& dbg = dbgs();

    dbg << "Contract extraction results" << endl;
    for (auto&& F : visitedFunctions) {
        auto&& memBounds = F->memBounds();
        std::vector<std::pair<PredicateState::Ptr, int>> choices;
        std::vector<PredicateState::Ptr> result;
        auto&& merger = MergingTransformer(FactoryNest(DL, nullptr), memBounds, F->calls());

        //magic number
        //if (F->calls() < 5) continue;

        //analyze each state
        for (auto&& st : *contracts->at(F)) {
            if (llvm::isa<PredicateStateChoice>(st)) {
                bool added = false;
                for (auto&& it_choice : choices) {
                    if (it_choice.first->equals(st.get())) {
                        ++it_choice.second;
                        added = true;
                        break;
                    }
                }
                if (not added) {
                    choices.push_back({st, 1});
                }
            } else {
                merger.transform(st);
            }
        }

        //collect results
        auto&& mergedState = merger.getMergedState();
        if (not mergedState->isEmpty()) result.push_back(mergedState);
        for (auto&& choice : choices) {
            if ((double)choice.second / F->calls() >= ContractManager::mergingConstant) {
                result.push_back(choice.first);
            }
        }

        //print results
        if (not result.empty()) {
            dbg << "---" << "Function " << F->name() << "---" << endl;

            for (auto&& state : result) {
                dbg << "State:" << endl;
                dbg << state << endl;
            }

            dbg << endl;
        }

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
ContractManager::FunctionSet ContractManager::visitedFunctions;

ContractManager::ContractStates ContractManager::summaries;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
