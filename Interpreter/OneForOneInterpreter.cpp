//
// Created by abdullin on 11/22/17.
//

#include "Interpreter/IR/Module.h"
#include "OneForOneInterpreter.h"
#include "State/Transformer/GlobalVariableFinder.h"
#include "State/Transformer/Interpreter.h"
#include "State/Transformer/PropertyPredicateFilterer.h"
#include "State/Transformer/QueryChecker.h"

namespace borealis {
namespace absint {

static config::BoolConfigEntry enableLogging("absint", "checker-logging");

OneForOneInterpreter::OneForOneInterpreter(const llvm::Instruction* I, SlotTrackerPass* ST, FactoryNest FN)
        : ObjectLevelLogging("ps-interpreter"), I_(I), ST_(ST), FN_(FN) {}

bool OneForOneInterpreter::check(PredicateState::Ptr state, PredicateState::Ptr query, const DefectInfo& di) {
    auto&& info = infos();
    auto module = absint::ir::Module(I_->getParent()->getParent()->getParent(), ST_);

    auto globFinder = GlobalVariableFinder(FN_);
    auto searched = globFinder.transform(state);
    auto filtered = PropertyPredicateFilterer(FN_).transform(searched);
    module.initGlobals(globFinder.getGlobals());

    if (enableLogging.get(false)) {
        info << "Function: " << I_->getParent()->getParent()->getName().str() << endl;
        info << "Instruction: " << ST_->toString(I_) << endl;
        info << "Defect: " << di << endl;
        info << "State: " << state << endl;
        info << "Query: " << query << endl;
        info << endl;
    }

    auto interpreter = absint::ps::Interpreter(FN_, module.getDomainFactory());
    interpreter.transform(filtered);

    auto checker = ps::QueryChecker(FN_, module.getDomainFactory(), interpreter.getState());
    checker.transform(query);

    if (enableLogging.get(false)) {
        info << "Query satisfied: " << checker.satisfied() << endl;
        info << endl;
    }

    return (not checker.satisfied());
}

}   // namespace absint
}   // namespace borealis