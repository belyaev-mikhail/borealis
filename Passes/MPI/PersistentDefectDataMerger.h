//
// Created by abdullin on 11/29/16.
//

#ifndef BOREALIS_PERSISTENTDEFECTDATAMERGER_H
#define BOREALIS_PERSISTENTDEFECTDATAMERGER_H

#include <llvm/Pass.h>

namespace borealis {

class PersistentDefectDataMerger : public llvm::ModulePass {
public:

    static char ID;

    PersistentDefectDataMerger();

    virtual bool runOnModule(llvm::Module& M) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

}   /* namespace borealis */

#endif //BOREALIS_PERSISTENTDEFECTDATAMERGER_H
