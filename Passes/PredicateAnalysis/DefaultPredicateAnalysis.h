/*
 * DefaultPredicateAnalysis.h
 *
 *  Created on: Aug 31, 2012
 *      Author: ice-phoenix
 */

#ifndef DEFAULTPREDICATEANALYSIS_H_
#define DEFAULTPREDICATEANALYSIS_H_

#include <llvm/Pass.h>
#include <llvm/Target/TargetData.h>

#include "Passes/PredicateAnalysis/AbstractPredicateAnalysis.h"
#include "Passes/Util/ProxyFunctionPass.h"
#include "Predicate/PredicateFactory.h"
#include "Term/TermFactory.h"
#include "Util/passes.hpp"

namespace borealis {

class DefaultPredicateAnalysis:
        public AbstractPredicateAnalysis,
        public ProxyFunctionPass,
        public ShouldBeModularized {

    friend class DPAInstVisitor;

public:

    static char ID;

    DefaultPredicateAnalysis();
    DefaultPredicateAnalysis(llvm::Pass*);
    virtual bool runOnFunction(llvm::Function& F) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& Info) const override;
    virtual ~DefaultPredicateAnalysis();

private:

    PredicateFactory::Ptr PF;
    TermFactory::Ptr TF;
    llvm::TargetData* TD;

};

} /* namespace borealis */

#endif /* DEFAULTPREDICATEANALYSIS_H_ */