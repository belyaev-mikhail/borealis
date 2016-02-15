/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <fstream>

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

void ContractManager::addSummary(llvm::Function* F, PredicateState::Ptr S,
                                 const Term::Ptr ret, const Term::Ptr implyTo) {
    summaries.push_back(Summary(F,S,ret,implyTo));
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
    if (auto&& basic = llvm::dyn_cast<BasicPredicateState>(state)) {
        Term::Ptr stateTerm = nullptr;
        for (auto&& pred : basic->getData()) {
            if (auto&& eq = llvm::dyn_cast<EqualityPredicate>(pred)) {
                auto &&predTerm = eq->getRhv()->equals(FN.Term->getFalseTerm().get()) ?
                                  FN.Term->getUnaryTerm(llvm::UnaryArithType::NOT, eq->getLhv()) :
                                  eq->getLhv();
                if (not stateTerm) stateTerm = predTerm;
                else stateTerm = FN.Term->getBinaryTerm(llvm::ArithType::BAND, stateTerm, predTerm);
            } else {
                return nullptr;
            }
        }
        return stateTerm;
    } else if (auto&& chain = llvm::dyn_cast<PredicateStateChain>(state)) {
        auto&& baseTerm = stateToTerm(chain->getBase());
        auto&& currTerm = stateToTerm(chain->getCurr());
        if (baseTerm && currTerm) return FN.Term->getBinaryTerm(llvm::ArithType::BAND, baseTerm, currTerm);
        else return nullptr;
    } else {
        auto&& choice = llvm::cast<PredicateStateChoice>(state);
        Term::Ptr choiceTerm = nullptr;
        for (auto&& st : choice->getChoices()) {
            if (auto&& stTerm = stateToTerm(st)) {
                if (not choiceTerm) choiceTerm = stTerm;
                else choiceTerm = FN.Term->getBinaryTerm(llvm::ArithType::BOR, choiceTerm, stTerm);
            } else {
                return nullptr;
            }
        }
        return choiceTerm;
    }
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

        dbg << "---" << "Function " << F->name() << "---" << endl;
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

        //magic number
        if (F->calls() < 5) continue;

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
        dbg << it.state;
        dbg << " imply " << it.retval << " to ";
        dbg << it.impTo << "\n\n\n\n";
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
