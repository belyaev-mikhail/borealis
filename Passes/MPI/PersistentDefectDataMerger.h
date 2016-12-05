//
// Created by abdullin on 11/29/16.
//

#ifndef BOREALIS_PERSISTENTDEFECTDATAMERGER_H
#define BOREALIS_PERSISTENTDEFECTDATAMERGER_H

#include <llvm/Pass.h>

namespace borealis {

class PersistentDefectDataMerger : public llvm::ModulePass {
public:

    using Data = impl_::persistentDefectData::SimpleT;

    static char ID;
    static const std::string defectFile;

    PersistentDefectDataMerger();

    virtual bool runOnModule(llvm::Module& M) override;
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
};

}   /* namespace borealis */

#endif //BOREALIS_PERSISTENTDEFECTDATAMERGER_H
