//
// Created by abdullin on 11/29/16.
//

#include "Config/config.h"
#include "Driver/mpi_driver.h"
#include "Passes/Defect/DefectManager.h"
#include "Util/passes.hpp"

#include "PersistentDefectDataMerger.h"

#include "Util/macros.h"

namespace borealis {

static config::BoolConfigEntry uniquePersistentDefectData("analysis", "unique-persistent-defect-data");

PersistentDefectDataMerger::PersistentDefectDataMerger() : ModulePass(ID), globalReady(false) {}

void PersistentDefectDataMerger::mergeLocal(Data& pdd) {
    // merge local persistentDefectData
    if (driver_.isLocalRoot()) {
        std::ifstream own("persistent" + std::to_string(driver_.getRank()) + ".json");
        if (auto&& loaded = util::read_as_json<Data>(own)) merge(pdd, *loaded);

        for (auto i = 1; i < driver_.getNodeSize(); ++i) {
            auto&& msg = driver_.receiveInteger(mpi::MPI_Driver::ANY);
            ASSERTC(msg.getTag() == mpi::Tag::READY);
            if (driver_.getStatus().source_ == mpi::Rank::ROOT) {
                globalReady = true;
                --i;
            } else {
                std::ifstream in("persistent" + std::to_string(driver_.getStatus().source_) + ".json");
                if (auto &&loaded = util::read_as_json<Data>(in)) merge(pdd, *loaded);
            }
        }
    } else {
        driver_.sendInteger(mpi::Rank::ROOT, {mpi::MPI_Driver::ANY, mpi::Tag::READY});
    }
}

void PersistentDefectDataMerger::mergeGlobal(Data& pdd) {
    // merge persistentDefectData on nodes
    if (driver_.isRoot()) {
        const int numOfNodes = driver_.getSize() / driver_.getNodeSize();

        std::ifstream in{defectFile};
        auto&& data = util::read_as_json<Data>(in);
        if (data) merge(pdd, *data);

        // say to local roots that global root is ready
        for(auto i = driver_.getNodeSize(); i < driver_.getSize(); i += driver_.getNodeSize())
            driver_.sendInteger(i, {mpi::MPI_Driver::ANY, mpi::Tag::READY});

        for(auto&& i = 1; i < numOfNodes; ++i) merge(pdd, receiveDefects());
        for(auto i = driver_.getNodeSize(); i < driver_.getSize(); i += driver_.getNodeSize()) sendDefects(i, pdd);

    } else if (driver_.isLocalRoot()) {
        if (not globalReady) {
            auto&& msg = driver_.receiveInteger(mpi::Rank::ROOT);
            ASSERTC(msg.getTag() == mpi::Tag::READY);
        }
        sendDefects(mpi::Rank::ROOT, pdd);
        pdd = receiveDefects();
    }
}

bool PersistentDefectDataMerger::runOnModule(llvm::Module& M) {
    if (uniquePersistentDefectData.get(false)) {
        auto& dm = GetAnalysis<DefectManager>::doit(this);
        dm.doFinalization(M);

        Data pdd;
        mergeLocal(pdd);
        if (driver_.isLocalRoot()) {
            mergeGlobal(pdd);
            dump(pdd);
        }
    }
    return false;
}

void PersistentDefectDataMerger::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
    AU.setPreservesAll();

    AUX<DefectManager>::addRequiredTransitive(AU);
}

void PersistentDefectDataMerger::merge(Data &to, const Data &from) {
    to.first.insert(std::make_move_iterator(from.first.begin()),
                    std::make_move_iterator(from.first.end()));
    to.second.insert(std::make_move_iterator(from.second.begin()),
                     std::make_move_iterator(from.second.end()));
}

PersistentDefectDataMerger::Data PersistentDefectDataMerger::receiveDefects() {
    auto&& dataBytes = driver_.receiveBytesArray();
    std::istringstream in(dataBytes.getData());
    auto&& data = util::read_as_json<Data>(in);
    return (data) ? std::move(*data) : Data();
}

void PersistentDefectDataMerger::sendDefects(mpi::Rank rank, Data& defects) {
    std::stringstream json;
    util::write_as_json(json, defects);
    driver_.sendBytesArray(rank, {json.str(), mpi::Tag::DataTag::BYTEARRAY} );
}

void PersistentDefectDataMerger::dump(Data& defects) {
    std::ofstream out{ defectFile };
    util::write_as_json(out, defects);
}


const std::string PersistentDefectDataMerger::defectFile = "persistentDefectData.json";

char PersistentDefectDataMerger::ID;
static RegisterPass<PersistentDefectDataMerger>
        X("defect-data-merger", "Pass that merges persistent defect data of all borealis instances");

}   /* namespace borealis */

#include "Util/unmacros.h"