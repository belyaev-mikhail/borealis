/*
 * clang_pipeline.h
 *
 *  Created on: Aug 20, 2013
 *      Author: belyaev
 */

#ifndef CLANG_PIPELINE_H_
#define CLANG_PIPELINE_H_

#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/Utils.h>

#include <llvm/ADT/IntrusiveRefCntPtr.h>

#include "Driver/AnnotatedModule.h"
#include "Driver/cl.h"
#include "Driver/interviewer.h"
#include "Logging/logger.hpp"
#include "TestGen/FunctionsInfoData.h"

namespace borealis {
namespace driver {

class clang_pipeline: public logging::DelegateLogging {
    struct impl;
    std::unique_ptr<impl> pimpl;

public:
    enum class status { SUCCESS, FAILURE };

    clang_pipeline(
        const std::string& what,
        const llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine>& diags
    );
    ~clang_pipeline();

    void invoke(const command&, bool forModify = false);
    void invoke(const std::vector<command>&, bool forModify = false);
    AnnotatedModule::Ptr result();
    
    FunctionsInfoData* getFunctionsInfoData();
};

} // namespace driver
} // namespace borealis

#endif /* CLANG_PIPELINE_H_ */
