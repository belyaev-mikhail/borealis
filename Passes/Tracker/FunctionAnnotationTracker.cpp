/*
 * AnnotatorPass.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: belyaev
 */

#include "Annotation/AnnotationCast.h"
#include "Annotation/EnsuresAnnotation.h"
#include "Annotation/RequiresAnnotation.h"
#include "Factory/Nest.h"
#include "Passes/Tracker/FunctionAnnotationTracker.h"
#include "Util/passes.hpp"
#include "Util/util.h"

namespace borealis {

void FunctionAnnotationTracker::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();

    AUX< SourceLocationTracker >::addRequiredTransitive(AU);
    AUX< AnnotationManager >::addRequiredTransitive(AU);
}

bool FunctionAnnotationTracker::runOnModule(llvm::Module&) {
    using borealis::util::view;

    SourceLocationTracker& SLT = GetAnalysis< SourceLocationTracker >::doit(this);
    AnnotationManager& AM = GetAnalysis< AnnotationManager >::doit(this);

    for(auto A : AM ) {
        if(!llvm::isa<EnsuresAnnotation>(A) && !llvm::isa<RequiresAnnotation>(A)) continue;

        auto&& loc = A->getLocus();
        for(auto&& v: util::view(SLT.getRangeFor(loc))) {
            if(auto&& f = llvm::dyn_cast<llvm::Function>(v.second)) {
                annotations[f].push_back(A);
                break;
            }
        }
    }

    return false;
}

void FunctionAnnotationTracker::print(llvm::raw_ostream&, const llvm::Module*) const {
    for (const auto& An : annotations) {
        infos() << An << endl;
    }
}

char FunctionAnnotationTracker::ID;
static RegisterPass<FunctionAnnotationTracker>
X("function-annotation-tracker", "Pass that tracks annotations bound to functions", false, false);

} // namespace borealis
