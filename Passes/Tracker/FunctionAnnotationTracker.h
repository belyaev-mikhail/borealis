/*
 * FunctionAnnotationTracker.h
 *
 *  Created on: Oct 8, 2012
 *      Author: belyaev
 */

#ifndef FUNCTIONANNOTATIONTRACKER_H
#define FUNCTIONANNOTATIONTRACKER_H

#include <llvm/Pass.h>

#include <vector>

#include "Passes/Manager/AnnotationManager.h"
#include "Passes/Tracker/SourceLocationTracker.h"

namespace borealis {

class FunctionAnnotationTracker:
    public llvm::ModulePass,
    public borealis::logging::ClassLevelLogging<FunctionAnnotationTracker> {

public:
    static char ID;

#include "Util/macros.h"
    static constexpr auto loggerDomain() QUICK_RETURN("function-annotations")
#include "Util/unmacros.h"

    FunctionAnnotationTracker() : llvm::ModulePass(ID) {};
    virtual ~FunctionAnnotationTracker() {};

    std::unordered_map<llvm::Function*, std::vector<Annotation::Ptr>> annotations;

public:

#include "Util/macros.h"
    auto begin() QUICK_RETURN(annotations.begin())
    auto end() QUICK_RETURN(annotations.end())
#include "Util/unmacros.h"

    const std::vector<Annotation::Ptr>& getAnnotations(llvm::Function& f) const {
        return getAnnotations(&f);
    }
    const std::vector<Annotation::Ptr>& getAnnotations(llvm::Function* f) const {
        static const std::vector<Annotation::Ptr> empty;
        auto it = annotations.find(f);
        if(it == std::end(annotations)) return empty;
        return it -> second;
    }

    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;
    virtual bool runOnModule(llvm::Module&) override;
    virtual void print(llvm::raw_ostream&, const llvm::Module*) const override;
};

} // namespace borealis

#endif // FUNCTIONANNOTATIONTRACKER_H
