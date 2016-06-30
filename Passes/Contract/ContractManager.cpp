/*
 * ContractManager.cpp
 *
 *  Created on: 21 мая 2015 г.
 *      Author: kivi
 */

#include <fstream>
#include <Codegen/intrinsics_manager.h>
#include <DB.hpp>

#include "ContractManager.h"
#include "Database/SerialTemplateSpec.hpp"
#include "SMT/Z3/Z3.h"
#include "SMT/Z3/Solver.h"
#include "State/Transformer/Retyper.h"
#include "State/Transformer/ArgumentUnifier.h"
#include "State/Transformer/StateOptimizer.h"
#include "State/Transformer/MergingTransformer.h"
#include "State/Transformer/ChoiceOptimizer.h"
#include "Util/passes.hpp"


namespace borealis {

ContractManager::ContractManager() : ModulePass(ID) {}

bool ContractManager::runOnModule(llvm::Module& M) {
    const llvm::DataLayout* DL = M.getDataLayout();
    FN = FactoryNest(DL, nullptr);
    VariableInfoTracker* VI = &GetAnalysis<VariableInfoTracker>::doit(this);
    FN.Type->initialize(*VI);

    if (not contracts) {
        contracts = ContractContainer::Ptr{new ContractContainer()};
    }

    return false;
}

void ContractManager::addContract(llvm::Function* F, FunctionManager& FM, PredicateState::Ptr S,
                                  const std::unordered_map<int, Args>& mapping) {
    if (IntrinsicsManager::getInstance().getIntrinsicType(F) != function_type::UNKNOWN)
        return;
    auto&& func = contracts->getFunctionId(F, FM.getMemoryBounds(F));
    func->called();
    functionInfo[func] = {F, &FM};

    auto&& unified = ArgumentUnifier(FN, mapping).transform(S);
    auto&& retyped = Retyper(FN).transform(unified);
    auto&& choiceOptimized = ChoiceOptimizer(FN).transform(retyped);
    auto&& optimized = StateOptimizer(FN).transform(choiceOptimized);
    if (not optimized->isEmpty()) {
        saveState(func, optimized);
    }
}

void ContractManager::addSummary(llvm::Function* F, PredicateState::Ptr S, FunctionManager& FM) {
    auto&& retyped = Retyper(FN).transform(S);
    auto&& choiceOptimized = ChoiceOptimizer(FN).transform(retyped);
    auto&& optimized = StateOptimizer(FN).transform(choiceOptimized);
    FM.update(F, optimized);
    summaries.push_back({F, optimized});
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
    } else {
        errs() << "Unable to save state:" << state << endl;
    }
}

Term::Ptr ContractManager::stateToTerm(PredicateState::Ptr state) const {
    Term::Ptr stateTerm = nullptr;
    if (auto&& basic = llvm::dyn_cast<BasicPredicateState>(state)) {
        for (auto&& pred : basic->getData()) {
            Term::Ptr predTerm = nullptr;

            if (auto&& equalityPr = llvm::dyn_cast<EqualityPredicate>(pred)) {
                predTerm = equalityPr->getRhv()->equals(FN.Term->getFalseTerm().get()) ?
                                  FN.Term->getUnaryTerm(llvm::UnaryArithType::NOT, equalityPr->getLhv()) :
                                  equalityPr->getLhv();

            } else if (auto&& switchPr = llvm::dyn_cast<DefaultSwitchCasePredicate>(pred)) {
                auto&& cond = switchPr->getCond();
                for (auto&& cs : switchPr->getCases()) {
                    auto&& caseTerm = FN.Term->getCmpTerm(llvm::ConditionType::NEQ, cond, cs);
                    predTerm = (not predTerm) ?
                               caseTerm :
                               FN.Term->getBinaryTerm(llvm::ArithType::BAND, predTerm, caseTerm);
                }

            } else {
                errs() << "Unable to transform predicate to term: " << pred << endl;
                return nullptr;
            }

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
            if (auto&& choiceTerm = stateToTerm(st)) {
                stateTerm = (not stateTerm) ?
                            choiceTerm :
                            FN.Term->getBinaryTerm(llvm::ArithType::BOR, stateTerm, choiceTerm);
            } else return nullptr;
        }
    }
    return stateTerm;
}

void ContractManager::printResults() {
    syncWithDB();
    applyContracts();
    printSummaries();
    //dump all found contracts
    printContractsDump();
}

void ContractManager::printContractsDump() const {
    auto&& dbg = dbgs();

    dbg << "Contracts dump" << endl;
    for (auto&& it : contracts->data()) {
        auto&& F = it.first;
        auto&& contracts = it.second;

        dbg << "---" << "Function " << F->name() << "---" << endl;
        dbg << "Called " << F->calls() << " times, found " << contracts->size() << " states" << endl;
        for (auto&& state : *contracts) {
            dbg << "State:" << endl;
            dbg << state << endl;
            dbg << endl;
        }

        dbg << endl;
    }

    dbg << end;
}

void ContractManager::applyContracts() const {
    auto&& dbg = dbgs();

    dbg << "Contract extraction results" << endl;
    for (auto&& it : contracts->data()) {
        auto&& F = it.first;
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

            if (auto&& contract = stateToTerm(mergedState)) {
                auto&& function = functionInfo[F].first;
                auto&& fm = functionInfo[F].second;
                auto&& contractPredicate = FN.Predicate->getEqualityPredicate(contract, FN.Term->getTrueTerm(), Locus(), PredicateType::REQUIRES);
                fm->update(function, FN.State->Basic({contractPredicate}));
            }

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
    dbg << end;
}

void ContractManager::syncWithDB() {
    auto&& db = leveldb_daemon::DB::getInstance();
    for (auto&& it : contracts->data()) {
        auto&& F = it.first;
        auto&& contract = it.second;

        std::string idKey = F->name() + F->rettype() + "_id";
        std::string contactKey = F->name() + F->rettype() + "_contract";

        db->lock();
        auto&& dbF = db->read<FunctionIdentifier, FactoryNest>(idKey, FN);
        auto&& dbContract = db->read<Contract, FactoryNest>(contactKey, FN);

        if (dbF) F->add(dbF.get());
        if (dbContract) contract->add(dbContract.get());

        db->write(idKey, *F);
        db->write(contactKey, *contract);
        db->unlock();
    }
}

ContractContainer::Ptr ContractManager::readFromDB() {
    auto&& db = leveldb_daemon::DB::getInstance();
    return db->read<ContractContainer, FactoryNest>(PROTOBUF_FILE, FN);
}

void ContractManager::writeToDB() const {
    auto&& db = leveldb_daemon::DB::getInstance();
    db->write(PROTOBUF_FILE, *contracts);
}

ContractContainer::Ptr ContractManager::readFromFile(const std::string &fname) {
    std::ifstream contractsStream(fname, std::iostream::in | std::iostream::binary);
    if (not contractsStream) {
        return nullptr;
    }

    ContractContainer::ProtoPtr proto{ new proto::ContractContainer{} };
    proto->ParseFromIstream(&contractsStream);

    return deprotobuffy(FN, *proto);
}

void ContractManager::writeToFile(const std::string &fname) const {
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
ContractManager::FunctionInfo ContractManager::functionInfo;

std::vector<ContractManager::Summary> ContractManager::summaries;

static llvm::RegisterPass<ContractManager>
X("contract-manager", "Contract manager pass", false, false);

} /* namespace borealis */
