//
// Created by abdullin on 11/29/16.
//

#ifndef BOREALIS_PERSISTENTDEFECTDATAMERGER_H
#define BOREALIS_PERSISTENTDEFECTDATAMERGER_H

#include <llvm/Pass.h>

#include "Passes/Defect/PersistentFunctionData.h"

namespace borealis {

class PersistentFunctionDataMerger : public llvm::ModulePass {
public:

    using Data = PersistentFunctionData::Data;

    static char ID;
    static const std::string defectFile_;
    static const std::string prefix_;
    static const std::string postfix_;

    PersistentFunctionDataMerger();

    virtual bool runOnModule(llvm::Module&) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;

private:

    void mergeLocal(Data& data);
    void mergeGlobal(Data& data);

    void merge(Data &to, const Data &from);
    Data receiveDefects();
    void sendDefects(mpi::Rank rank, Data& defects);
    void dump(Data& defects);

private:

    mpi::MPI_Driver driver_;
    bool globalReady_;
};

}   /* namespace borealis */

#endif //BOREALIS_PERSISTENTDEFECTDATAMERGER_H
