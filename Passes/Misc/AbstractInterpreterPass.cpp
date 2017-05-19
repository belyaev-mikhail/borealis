//
// Created by abdullin on 2/10/17.
//

#include <llvm/Pass.h>

#include "Interpreter/Interpreter.h"
#include "Util/passes.hpp"

namespace borealis {

class AbstractInterpreterPass : public llvm::ModulePass {

public:

    static char ID;

    AbstractInterpreterPass() : llvm::ModulePass(ID) {};

    virtual bool runOnModule(llvm::Module& M) override {
        absint::Interpreter interpreter(&M);
        interpreter.run();
        return false;
    }

    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override {
        AU.setPreservesAll();
    }

    virtual ~AbstractInterpreterPass() = default;

};

char AbstractInterpreterPass::ID;
static RegisterPass<AbstractInterpreterPass>
X("abs-interpreter", "Pass that performs abstract interpretation on a module");

} // namespace borealis

