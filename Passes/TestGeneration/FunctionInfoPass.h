/* 
 * File:   FunctionInfoPass.h
 * Author: maxim
 *
 * Created on 19 Май 2014 г., 13:29
 */

#ifndef FUNCTIONINFOPASS_H
#define	FUNCTIONINFOPASS_H

#include <fstream>
#include <llvm/Metadata.h>
#include <llvm/Pass.h>

#include "Driver/AnnotatedModule.h"
#include "Logging/logger.hpp"
#include "Passes/TestGeneration/TestManager.h"
#include "TestGen/FunctionInfo.h"

#include "Util/passes.hpp"

namespace borealis {

class FunctionInfoPass :
        public llvm::ModulePass,
        public borealis::logging::ClassLevelLogging<FunctionInfoPass> {
public:

    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("function-info")
#include "Util/unmacros.h"

    typedef std::unordered_map<const llvm::Function*, FunctionInfo> FunctionInfoMap;
    typedef std::shared_ptr<FunctionInfoMap> FunctionInfoMapPtr;
    
    FunctionInfoPass();
    
    virtual bool runOnModule(llvm::Module& M) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
    virtual ~FunctionInfoPass();
    
    FunctionInfo getFunctionInfo(const llvm::Function* F) const;
    
private:
    TestManager * tm;
    FunctionInfoMap functions;

};

} /* namespace borealis */

#endif	/* FUNCTIONINFOPASS_H */

