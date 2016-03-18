/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <fstream>
#include <Codegen/intrinsics_manager.h>

#include "SMT/Z3/Z3.h"
#include "SMT/Z3/Solver.h"
#include "Util/passes.hpp"
#include "Protobuf/Converter.hpp"
#include "State/Transformer/Retyper.h"
#include "State/Transformer/ArgumentUnifier.h"
#include "State/Transformer/StateOptimizer.h"
#include "State/Transformer/MergingTransformer.h"
#include "State/Transformer/ChoiceOptimizer.h"
#include "ContractManager.h"

namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::doFinalization(llvm::Module&) {
    writeTo(PROTOBUF_FILE);
    return false;
}

bool ContractManager::runOnModule(llvm::Module& M) {
    const llvm::DataLayout* DL = M.getDataLayout();
    FN = FactoryNest(DL, nullptr);
    VariableInfoTracker* VI = &GetAnalysis<VariableInfoTracker>::doit(this);
    FN.Type->initialize(*VI);

    contracts = readFrom(PROTOBUF_FILE);
    if (not contracts) {
        contracts = ContractContainer::Ptr{new ContractContainer()};
    }

    return false;
}

void ContractManager::addContract(llvm::Function* F, const FunctionManager& FM, PredicateState::Ptr S,
                                  const std::unordered_map<int, Args>& mapping) {
    if (IntrinsicsManager::getInstance().getIntrinsicType(F) != function_type::UNKNOWN)
        return;
    auto&& func = contracts->getFunctionId(F, FM.getMemoryBounds(F));
    func->called();
    visitedFunctions.insert(func);
    if (not S->isEmpty()) {
        auto&& unified = ArgumentUnifier(FN, mapping).transform(S);
        auto&& retyped = Retyper(FN).transform(unified);
        auto&& choiceOptimized = ChoiceOptimizer(FN).transform(retyped);
        auto&& optimized = StateOptimizer(FN).transform(choiceOptimized);
        if (not optimized->isEmpty()) {
            saveState(func, optimized);
        }
    }
}

void ContractManager::addSummary(llvm::Function* F, PredicateState::Ptr S, FunctionManager& FM) {
    auto&& retyped = Retyper(FN).transform(S);
    auto&& choiceOptimized = ChoiceOptimizer(FN).transform(retyped);
    auto&& optimized = StateOptimizer(FN).transform(choiceOptimized);
    FM.update(F, optimized);
    summaries.push_back(Summary(F, optimized));
}

void ContractManager::saveState(FunctionIdentifier::Ptr func, PredicateState::Ptr state) {
    if (llvm::isa<BasicPredicateState>(state)) {
        contracts->at(func)->push_back(state);
    } else if (auto&& chain = llvm::dyn_cast<PredicateStateChain>(state)) {
        saveState(func, chain->getBase());
        saveState(func, chain->getCurr());
    } else if (auto&& stateTerm = stateToTerm(state)) {
        auto&& statePred = FN.Predicate->getEqualityPredicate(stateTerm, FN.Term->getTrueTerm(), Locus(), PredicateType::PATH);
        auto&& newState = FN.State->Basic({statePred});

        Z3::ExprFactory ef;
        Z3::Solver s(ef, func->memBounds().first, func->memBounds().second);
        auto res = s.isFullGroup(newState);

        if (res.isSat()) {
            contracts->at(func)->push_back(newState);
        }
    }
}

Term::Ptr ContractManager::stateToTerm(PredicateState::Ptr state) {
    Term::Ptr stateTerm = nullptr;
    if (auto&& basic = llvm::dyn_cast<BasicPredicateState>(state)) {
        for (auto&& pred: util::viewContainer(basic->getData())
                .map(llvm::dyn_caster<EqualityPredicate>())
                .filter()) {
            auto&& predTerm = pred->getRhv()->equals(FN.Term->getFalseTerm().get()) ?
                              FN.Term->getUnaryTerm(llvm::UnaryArithType::NOT, pred->getLhv()) :
                              pred->getLhv();
            stateTerm = (not stateTerm) ?
                        predTerm :
                        FN.Term->getBinaryTerm(llvm::ArithType::BAND, stateTerm, predTerm);
        }
    } else if (auto&& chain = llvm::dyn_cast<PredicateStateChain>(state)) {
        auto&& baseTerm = stateToTerm(chain->getBase());
        auto&& currTerm = stateToTerm(chain->getCurr());
        stateTerm = (baseTerm && currTerm) ?
                    FN.Term->getBinaryTerm(llvm::ArithType::BAND, baseTerm, currTerm) :
                    nullptr;
    } else {
        auto&& choice = llvm::cast<PredicateStateChoice>(state);
        for (auto&& st : choice->getChoices()) {
            if (auto&&choiceTerm = stateToTerm(st)) {
                stateTerm = (not stateTerm) ?
                            choiceTerm :
                            FN.Term->getBinaryTerm(llvm::ArithType::BOR, stateTerm, choiceTerm);
            } else return nullptr;
        }
    }
    return stateTerm;
}

void ContractManager::print(llvm::raw_ostream&, const llvm::Module*) const {
    printContracts();
    printSummaries();
}

void ContractManager::printContracts() const {
    auto&& dbg = dbgs();

    dbg << "Found contracts dump" << endl;
    for (auto&& it : contracts->data()) {
        auto&& F = it.first;

        dbg << "---" << "Function " << F->name() << "---" << "called " << F->calls() << endl;
        dbg << "Found " << contracts->at(F)->size() << " states" << endl;
        for (auto&& state : *contracts->at(F)) {
            dbg << "State:" << endl;
            dbg << state << endl;
            dbg << endl;
        }
        dbg << endl;
    }

    dbg << "Contract extraction results" << endl;
    for (auto&& F : visitedFunctions) {
        auto&& memBounds = F->memBounds();
        auto&& merger = MergingTransformer(FN, memBounds, F->calls());

        if (F->calls() < MIN_FUNCTION_CALLS) continue;

        //analyze each state
        for (auto&& st : *contracts->at(F)) {
            merger.transform(st);
        }

        //collect results
        auto&& mergedState = merger.getMergedState();
        if (not mergedState->isEmpty()) {
            dbg << "---" << "Function " << F->name() << "---" << endl;
            dbg << "State:" << endl;
            dbg << mergedState << endl;
            dbg << endl;
        }
    }

    dbg << end;
}

void ContractManager::printSummaries() const {
    auto&& dbg = dbgs();
    llvm::Function* prev = nullptr;
    dbg << "Summary extraction results" << endl;
    for (auto&& it : summaries) {
        if (prev == nullptr) {
            dbg << "---" << "Function " << it.func->getName() << "---" << endl;
        } else {
            if (prev->getName() != it.func->getName()) {
                dbg << "---" << "Function " << it.func->getName() << "---" << endl;
            }
        }
        dbg << it.state<< endl << endl << endl;
        prev = it.func;
    }
    dbg << endl;
}

ContractContainer::Ptr ContractManager::readFrom(const std::string& fname) {
    std::ifstream contractsStream(fname, std::iostream::in | std::iostream::binary);
    if (not contractsStream) {
        return nullptr;
    }

    ContractContainer::ProtoPtr proto{new proto::ContractContainer{}};
    proto->ParseFromIstream(&contractsStream);

    ContractContainer::Ptr container = deprotobuffy(FN, *proto);
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

std::vector<ContractManager::Summary> ContractManager::summaries;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
