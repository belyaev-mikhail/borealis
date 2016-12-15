//
// Created by abdullin on 12/14/16.
//

#ifndef BOREALIS_PERSISTENTFUNCTIONDATA_H
#define BOREALIS_PERSISTENTFUNCTIONDATA_H


#include <llvm/Pass.h>
#include <llvm/Support/LockFileManager.h>

#include <unordered_set>

#include "Logging/logger.hpp"

namespace borealis {

class PersistentFunctionData : public llvm::ModulePass {

public:

    using Data = std::unordered_set<std::string>;

    static char ID;
    static const std::string filename_;
    static const std::string prefix_;
    static const std::string postfix_;

    PersistentFunctionData();
    ~PersistentFunctionData() = default;

    virtual bool runOnModule(llvm::Module&) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;

    void addFunction(const llvm::Function& F);
    bool hasFunction(const llvm::Function& F);
    void forceDump();

private:

    void read();

    template<class Body>
    void locked(Body body) {
        while(true) {
            llvm::LockFileManager fileLock(filename_);
            if(fileLock == llvm::LockFileManager::LFS_Shared) {
                fileLock.waitForUnlock();
                continue;
            }
            if(fileLock == llvm::LockFileManager::LFS_Error) {
                errs() << "error while trying to lock file \"" << filename_ << "\"" << endl;
            }

            body();
            break;
        }
    }

private:

    static Data functionData_;
    static Data oldFunctionData_;

};

}   /* namespace borealis */

#endif //BOREALIS_PERSISTENTFUNCTIONDATA_H
