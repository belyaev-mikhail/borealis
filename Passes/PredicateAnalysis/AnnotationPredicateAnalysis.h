/*
 * AnnotationPredicateAnalysis.h
 *
 *  Created on: Feb 25, 2013
 *      Author: ice-phoenix
 */

#ifndef ANNOTATIONPREDICATEANALYSIS_H_
#define ANNOTATIONPREDICATEANALYSIS_H_

#include <llvm/Pass.h>

#include "Passes/PredicateAnalysis/AbstractPredicateAnalysis.h"
#include "Passes/Tracker/MetaInfoTracker.h"
#include "Passes/Util/ProxyFunctionPass.h"
#include "Predicate/PredicateFactory.h"
#include "Term/TermFactory.h"
#include "Util/passes.hpp"

namespace borealis {

class AnnotationPredicateAnalysis:
        public AbstractPredicateAnalysis,
        public ProxyFunctionPass,
        public ShouldBeModularized {

    friend class APAInstVisitor;

public:

    static char ID;

    AnnotationPredicateAnalysis();
    AnnotationPredicateAnalysis(llvm::Pass*);
    virtual bool runOnFunction(llvm::Function& F) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;
    virtual ~AnnotationPredicateAnalysis();

private:

    MetaInfoTracker* MI;
    PredicateFactory::Ptr PF;
    TermFactory::Ptr TF;

};

} /* namespace borealis */

#endif /* ANNOTATIONPREDICATEANALYSIS_H_ */
