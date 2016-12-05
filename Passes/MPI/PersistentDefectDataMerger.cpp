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

PersistentDefectDataMerger::PersistentDefectDataMerger() : ModulePass(ID) {}

bool PersistentDefectDataMerger::runOnModule(llvm::Module& M) {
    if (uniquePersistentDefectData.get(false)) {
        auto& dm = GetAnalysis<DefectManager>::doit(this);
        dm.doFinalization(M);

        mpi::MPI_Driver driver{};

        if (driver.isRoot()) {
            using Data = impl_::persistentDefectData::SimpleT;
            std::ifstream own("persistent" + std::to_string(driver.getRank()) + ".json");
            auto&& pdd = util::read_as_json<Data>(own);
            ASSERTC(pdd);

            for (auto i = 1; i < driver.getSize(); ++i) {
                auto&& msg = driver.receive(mpi::MPI_Driver::ANY);
                ASSERTC(msg.getTag() == mpi::Tag::READY);
                std::ifstream in("persistent" + std::to_string(driver.getStatus().source_) + ".json");
                if (auto&& loaded = util::read_as_json<Data>(in)) {
                    pdd->first.insert(std::make_move_iterator(loaded->first.begin()),
                                      std::make_move_iterator(loaded->first.end()));
                    pdd->second.insert(std::make_move_iterator(loaded->second.begin()),
                                       std::make_move_iterator(loaded->second.end()));
                }
            }

            {
                std::ifstream in{"persistentDefectData.json"};
                if (auto existing = util::read_as_json<Data>(in)) {
                    pdd->first.insert(std::make_move_iterator(existing->first.begin()),
                               std::make_move_iterator(existing->first.end()));
                    pdd->second.insert(std::make_move_iterator(existing->second.begin()),
                               std::make_move_iterator(existing->second.end()));
                }
            }
            {
                std::ofstream out{"persistentDefectData.json"};
                util::write_as_json(out, *pdd);
            }
        } else {
            driver.send(mpi::Rank::ROOT, {mpi::MPI_Driver::ANY, mpi::Tag::READY});
        }
    }
    return false;
}

void PersistentDefectDataMerger::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
    AU.setPreservesAll();

    AUX<DefectManager>::addRequiredTransitive(AU);
}

char PersistentDefectDataMerger::ID;
static RegisterPass<PersistentDefectDataMerger>
        X("defect-data-merger", "Pass that merges persistent defect data of all borealis instances");

}   /* namespace borealis */

#include "Util/unmacros.h"