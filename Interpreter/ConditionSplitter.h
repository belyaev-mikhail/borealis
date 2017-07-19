//
// Created by abdullin on 6/9/17.
//

#ifndef BOREALIS_CONDITIONSPLITTER_H
#define BOREALIS_CONDITIONSPLITTER_H

#include <map>

#include "Annotation/Annotation.h"
#include "Interpreter.h"
#include "Interpreter/Domain/Domain.h"
#include "Logging/logger.hpp"
#include "State.h"

namespace borealis {
namespace absint {

class ConditionSplitter : public logging::ObjectLevelLogging<ConditionSplitter> {

    using ValueMap = std::map<const llvm::Value*, Split>;

public:

    ConditionSplitter(llvm::Value* target, Interpreter* interpreter, State::Ptr state);

    ValueMap apply();

private:

    void handleICmp(llvm::Value* lhv, llvm::Value* rhv, const llvm::ICmpInst::Predicate& predicate);
    void handleFCmp(llvm::Value* lhv, llvm::Value* rhv, const llvm::FCmpInst::Predicate& predicate);
    void handleBinary(llvm::Value* lhv, llvm::Value* rhv, const llvm::BinaryOperator::BinaryOps);

    llvm::Value* target_;
    Interpreter* interpreter_;
    State::Ptr state_;
    ValueMap values_;

};

}   /* namespace absint */
}   /* namespace borealis */

#endif //BOREALIS_CONDITIONSPLITTER_H