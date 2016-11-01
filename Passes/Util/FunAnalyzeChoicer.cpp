//
// Created by stepanov on 9/13/16.
//

#include <llvm/Pass.h>

#include "Util/FunChoicer.h"
#include "Util/passes.hpp"

namespace borealis {


struct FunAnalyzeChoice : public llvm::FunctionPass
{
    static char ID;
    FunChoicer& fc = FunChoicer::Instance();

    FunAnalyzeChoice() : llvm::FunctionPass(ID){
    }

    virtual bool runOnFunction(llvm::Function& F) {
        if(fc.isAnalyze(&F)){
            F.deleteBody();
        }
        return false;
    }

    virtual ~FunAnalyzeChoice() {}
};

char FunAnalyzeChoice::ID;
static RegisterPass<FunAnalyzeChoice>
        X("fun-choicer", "Pass that choose functions that will be analyzed");

} // namespace borealis