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

#include "Logging/logger.hpp"
#include "Driver/cl.h"
#include "Driver/AnnotatedModule.h"
#include "Driver/interviewer.h"

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

    void invoke(const command&);
    void invoke(const std::vector<command>&);
    AnnotatedModule::Ptr result();
};

} // namespace driver
} // namespace borealis

#endif /* CLANG_PIPELINE_H_ */
