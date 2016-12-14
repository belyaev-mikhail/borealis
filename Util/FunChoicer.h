//
// Created by stepanov on 9/13/16.
//

#ifndef BOREALIS_FUNCHOICER_H
#define BOREALIS_FUNCHOICER_H

#include <llvm/Pass.h>

#include "Util/passes.hpp"

namespace borealis {

class FunChoicer {
public:


    static FunChoicer& Instance(){
        static FunChoicer singleInst;
        return singleInst;
    }

    bool isAnalyze(llvm::Function *F) {
        if ((int) F->getName().front() % 2 != 0)
            return false;
        else {
            anFun.push_back(F);
            return true;
        }
    }

    std::vector<llvm::Function *> getAnFun() { return anFun; }

private:
    FunChoicer() {};
    ~FunChoicer() {};

    FunChoicer(FunChoicer const&) = delete;
    FunChoicer& operator= (FunChoicer const&) = delete;

    std::vector<llvm::Function *> anFun;
};
}

#endif //BOREALIS_FUNCHOICER_H
