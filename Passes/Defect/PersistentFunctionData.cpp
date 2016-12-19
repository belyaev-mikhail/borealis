//
// Created by abdullin on 12/14/16.
//

#include <llvm/IR/Function.h>
#include <fstream>
#include <Driver/mpi_driver.h>

#include "Config/config.h"
#include "Util/passes.hpp"

#include "PersistentFunctionData.h"

namespace borealis {

static config::BoolConfigEntry usePersistentFunctionData("analysis", "persistent-function-data");

PersistentFunctionData::PersistentFunctionData() : ModulePass(ID) {
    read();
}

bool PersistentFunctionData::runOnModule(llvm::Module&) {
    return false;
}

void PersistentFunctionData::addFunction(const llvm::Function& F) {
    functionData_.insert(F.getName());
}

bool PersistentFunctionData::hasFunction(const llvm::Function& F) {
    return (functionData_.find(F.getName()) != functionData_.end()) ||
           (oldFunctionData_.find(F.getName()) != oldFunctionData_.end());
}

void PersistentFunctionData::read() {
    if (usePersistentFunctionData.get(false)) {
        std::ifstream in(filename_);
        if (auto&& pfd = util::read_as_json<Data>(in)) {
            oldFunctionData_ = std::move(*pfd);
        }
    }
}

void PersistentFunctionData::forceDump() {
    if (usePersistentFunctionData.get(false)) {
        mpi::MPI_Driver driver{};
        std::ofstream out{prefix_ + std::to_string(driver.getRank()) + postfix_};
        util::write_as_json(out, std::move(functionData_));
    }
}

void PersistentFunctionData::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
    AU.setPreservesAll();
}

char PersistentFunctionData::ID;

const std::string PersistentFunctionData::filename_ = "persistentFunctionData.json";
const std::string PersistentFunctionData::prefix_ = "persistent";
const std::string PersistentFunctionData::postfix_ = ".json";

PersistentFunctionData::Data PersistentFunctionData::functionData_;
PersistentFunctionData::Data PersistentFunctionData::oldFunctionData_;

static RegisterPass<PersistentFunctionData>
        X("function-data", "Pass that collects and serialize persistent function data");

}   /* namespace borealis */