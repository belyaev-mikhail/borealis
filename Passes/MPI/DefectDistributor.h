//
// Created by stepanov on 11/28/16.
//

#ifndef BOREALIS_DEFECTDISTRIBUTOR_H
#define BOREALIS_DEFECTDISTRIBUTOR_H

#include "Driver/mpi_driver.h"
#include "Passes/Defect/DefectManager/DefectInfo.h"
#include "Passes/Defect/DefectManager.h"


#include <llvm/IR/DebugInfo.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Pass.h>
#include <vector>

namespace borealis{

class DefectDistributor : public llvm::ModulePass {

    using DefectData = std::unordered_set<DefectInfo>;
    using SimpleT =  std::pair< DefectData, DefectData>;

public:
    static char ID;

    DefectDistributor() : llvm::ModulePass(ID), jsonFileName("persistentDefectData.json"), driver() {}
    virtual bool runOnModule(llvm::Module&) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
    virtual ~DefectDistributor() {}

    void receiveDefects(SimpleT& defects);
    void sendDefects(SimpleT& defects, mpi::Rank recRank);

    void dumpDefectsInFile(SimpleT& defects);

private:

    std::string jsonFileName;
    DefectManager* DM;
    mpi::MPI_Driver driver;
};

} // namespace Borealis

#endif //BOREALIS_DEFECTDISTRIBUTOR_H
