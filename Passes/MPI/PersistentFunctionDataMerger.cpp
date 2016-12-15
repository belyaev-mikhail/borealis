//
// Created by abdullin on 11/29/16.
//

#include "Config/config.h"
#include "Driver/mpi_driver.h"
#include "Passes/Defect/DefectManager.h"
#include "Util/passes.hpp"

#include "PersistentFunctionDataMerger.h"

#include "Util/macros.h"

namespace borealis {

static config::BoolConfigEntry usePersistentFunctionData("analysis", "persistent-function-data");

PersistentFunctionDataMerger::PersistentFunctionDataMerger() : ModulePass(ID), globalReady_(false) {}

void PersistentFunctionDataMerger::mergeLocal(Data& pdd) {
    // merge local persistentDefectData
    if (driver_.isLocalRoot()) {
        std::ifstream own(prefix_ + std::to_string(driver_.getRank()) + postfix_);
        if (auto&& loaded = util::read_as_json<Data>(own)) merge(pdd, *loaded);

        for (auto i = 1; i < driver_.getNodeSize(); ++i) {
            auto&& msg = driver_.receiveInteger(mpi::MPI_Driver::ANY);
            ASSERTC(msg.getTag() == mpi::Tag::READY);
            if (driver_.getStatus().source_ == mpi::Rank::ROOT) {
                globalReady_ = true;
                --i;
            } else {
                std::ifstream in(prefix_ + std::to_string(driver_.getStatus().source_) + postfix_);
                if (auto &&loaded = util::read_as_json<Data>(in)) merge(pdd, *loaded);
            }
        }
    } else {
        driver_.sendInteger(driver_.getLocalRoot(), {mpi::MPI_Driver::ANY, mpi::Tag::READY});
    }
}

void PersistentFunctionDataMerger::mergeGlobal(Data& pdd) {
    // merge persistentDefectData on nodes
    if (driver_.isRoot()) {
        const int numOfNodes = driver_.getSize() / driver_.getNodeSize();

        std::ifstream in{defectFile_};
        auto&& data = util::read_as_json<Data>(in);
        if (data) merge(pdd, *data);

        // say to local roots that global root is ready
        for (auto i = driver_.getNodeSize(); i < driver_.getSize(); i += driver_.getNodeSize())
            driver_.sendInteger(i, {mpi::MPI_Driver::ANY, mpi::Tag::READY});

        for(auto i = 1; i < numOfNodes; ++i) merge(pdd, receiveDefects());

    } else if (driver_.isLocalRoot()) {
        if (not globalReady_) {
            auto&& msg = driver_.receiveInteger(mpi::Rank::ROOT);
            ASSERTC(msg.getTag() == mpi::Tag::READY);
        }
        sendDefects(mpi::Rank::ROOT, pdd);
    }
    std::stringstream json;
    util::write_as_json(json, pdd);
    auto&& res = driver_.broadcastBytesArray({json.str(), mpi::Tag::DataTag::BYTEARRAY});
    if (driver_.isLocalRoot()) {
        std::istringstream in(res.getData());
        auto&& data = util::read_as_json<Data>(in);
        if (data) pdd = *data;
    }
}

bool PersistentFunctionDataMerger::runOnModule(llvm::Module&) {
    if (usePersistentFunctionData.get(false)) {
        auto& pfd = GetAnalysis<PersistentFunctionData>::doit(this);
        pfd.forceDump();

        Data pdd;
        mergeLocal(pdd);
        mergeGlobal(pdd);
        if (driver_.isLocalRoot())
            dump(pdd);
    }
    return false;
}

void PersistentFunctionDataMerger::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
    AU.setPreservesAll();

    AUX<PersistentFunctionData>::addRequiredTransitive(AU);
}

void PersistentFunctionDataMerger::merge(Data &to, const Data &from) {
    to.insert(std::make_move_iterator(from.begin()),
              std::make_move_iterator(from.end()));
}

PersistentFunctionDataMerger::Data PersistentFunctionDataMerger::receiveDefects() {
    auto&& dataBytes = driver_.receiveBytesArray();
    std::istringstream in(dataBytes.getData());
    auto&& data = util::read_as_json<Data>(in);
    return (data) ? std::move(*data) : Data();
}

void PersistentFunctionDataMerger::sendDefects(mpi::Rank rank, Data& defects) {
    std::stringstream json;
    util::write_as_json(json, defects);
    driver_.sendBytesArray(rank, {json.str(), mpi::Tag::DataTag::BYTEARRAY} );
}

void PersistentFunctionDataMerger::dump(Data& defects) {
    std::ofstream out{ defectFile_ };
    util::write_as_json(out, defects);
}

const std::string PersistentFunctionDataMerger::defectFile_ = PersistentFunctionData::filename_;
const std::string PersistentFunctionDataMerger::prefix_ = PersistentFunctionData::prefix_;
const std::string PersistentFunctionDataMerger::postfix_ = PersistentFunctionData::postfix_;

char PersistentFunctionDataMerger::ID;
static RegisterPass<PersistentFunctionDataMerger>
        X("defect-data-merger", "Pass that merges persistent defect data of all borealis instances");

}   /* namespace borealis */

#include "Util/unmacros.h"