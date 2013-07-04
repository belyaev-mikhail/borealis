/*
 * intrinsics_registry.cpp
 *
 *  Created on: Feb 18, 2013
 *      Author: ice-phoenix
 */

#include <llvm/Analysis/MemoryBuiltins.h>

#include "Codegen/intrinsics_manager.h"
#include "State/PredicateStateFactory.h"

#include "Util/macros.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpredefined-identifier-outside-function"

namespace borealis {

typedef IntrinsicsManager::RegisterIntrinsic RegisterIntrinsic;

// TODO: akhin Make generator accept PredicateStateFactory from the outside

static RegisterIntrinsic INTRINSIC_PTR_VERSION {
    function_type::INTRINSIC_PTR_VERSION,
    "ptrver",
    [](llvm::Function* F, PredicateFactory* PF, TermFactory* TF) -> PredicateState::Ptr {
        ASSERTC(F->getArgumentList().size() > 0);
        return PredicateStateFactory::get()->Basic() +
               PF->getEqualityPredicate(
                   TF->getReturnValueTerm(F),
                   TF->getArgumentTerm(F->getArgumentList().begin())
               );
    }
};

static RegisterIntrinsic INTRINSIC_VALUE {
    function_type::INTRINSIC_VALUE,
    "value"
};

static RegisterIntrinsic INTRINSIC_GLOBAL_DESCRIPTOR_TABLE {
    function_type::INTRINSIC_GLOBAL_DESCRIPTOR_TABLE,
    "globals"
};

static RegisterIntrinsic INTRINSIC_GLOBAL {
    function_type::INTRINSIC_GLOBAL,
    "global"
};

static RegisterIntrinsic INTRINSIC_ANNOTATION {
    function_type::INTRINSIC_ANNOTATION,
    "annotation"
};

static RegisterIntrinsic INTRINSIC_MALLOC {
    function_type::INTRINSIC_MALLOC,
    "malloc",
    [](llvm::Function* F, PredicateFactory* PF, TermFactory* TF) -> PredicateState::Ptr {
        ASSERTC(F->getArgumentList().size() > 0);
        return PredicateStateFactory::get()->Basic() +
               PF->getMallocPredicate(
                   TF->getReturnValueTerm(F),
                   TF->getArgumentTerm(F->getArgumentList().begin())
               );
    }
};

static RegisterIntrinsic BUILTIN_BOR_ASSERT {
    function_type::BUILTIN_BOR_ASSERT,
    "borealis_assert",
    [](llvm::Function* F, PredicateFactory* PF, TermFactory* TF) -> PredicateState::Ptr {
        ASSERTC(F->getArgumentList().size() > 0);
        return PredicateStateFactory::get()->Basic() +
               PF->getInequalityPredicate(
                    TF->getArgumentTerm(F->getArgumentList().begin()),
                    TF->getIntTerm(0ULL),
                    PredicateType::REQUIRES
               );
    },
    [](const IntrinsicsManager&, const llvm::CallInst& ci) {
        return ci.getCalledFunction()->getName() == "borealis_assert"
               ? function_type::BUILTIN_BOR_ASSERT
               : function_type::UNKNOWN;
    }
};

static RegisterIntrinsic BUILTIN_BOR_ASSUME {
    function_type::BUILTIN_BOR_ASSUME,
    "borealis_assume",
    [](llvm::Function* F, PredicateFactory* PF, TermFactory* TF) -> PredicateState::Ptr {
        ASSERTC(F->getArgumentList().size() > 0);
        return PredicateStateFactory::get()->Basic() +
               PF->getInequalityPredicate(
                    TF->getArgumentTerm(F->getArgumentList().begin()),
                    TF->getIntTerm(0ULL),
                    PredicateType::ENSURES
               );
    },
    [](const IntrinsicsManager&, const llvm::CallInst& ci) {
        return ci.getCalledFunction()->getName() == "borealis_assume"
               ? function_type::BUILTIN_BOR_ASSUME
               : function_type::UNKNOWN;
    }
};

static RegisterIntrinsic ACTION_DEFECT {
    function_type::ACTION_DEFECT,
    "borealis_action_defect",
    RegisterIntrinsic::DefaultGenerator,
    [](const IntrinsicsManager&, const llvm::CallInst& ci) {
        return ci.getCalledFunction()->getName() == "borealis_action_defect"
               ? function_type::ACTION_DEFECT
               : function_type::UNKNOWN;
    }
};

static RegisterIntrinsic BUILTIN_BOR_GETPROP {
    function_type::BUILTIN_BOR_GETPROP,
    "borealis_get_property",
    [](llvm::Function* F, PredicateFactory* PF, TermFactory* TF) -> PredicateState::Ptr {
        ASSERTC(F->getArgumentList().size() > 1);

        auto it = F->arg_begin();
        llvm::Argument* propName = it++;
        llvm::Argument* ptr = it++;

        return PredicateStateFactory::get()->Basic() +
               PF->getEqualityPredicate(
                    TF->getReturnValueTerm(F),
                    TF->getReadPropertyTerm(
                            TF->getArgumentTerm(propName),
                            TF->getArgumentTerm(ptr),
                            TypeFactory::getInstance().cast(F->getReturnType())
                    )
               );
    },
    [](const IntrinsicsManager&, const llvm::CallInst& ci) {
        return ci.getCalledFunction()->getName() == "borealis_get_property"
               ? function_type::BUILTIN_BOR_GETPROP
               : function_type::UNKNOWN;
    }
};

static RegisterIntrinsic BUILTIN_BOR_SETPROP {
    function_type::BUILTIN_BOR_SETPROP,
    "borealis_set_property",
    [](llvm::Function* F, PredicateFactory* PF, TermFactory* TF) -> PredicateState::Ptr {
        ASSERTC(F->getArgumentList().size() > 2);

        auto it = F->arg_begin();
        llvm::Argument* propName = it++;
        llvm::Argument* ptr = it++;
        llvm::Argument* value = it++;

        return PredicateStateFactory::get()->Basic() +
               PF->getWritePropertyPredicate(
                        TF->getArgumentTerm(propName),
                        TF->getArgumentTerm(ptr),
                        TF->getArgumentTerm(value)
               );
    },
    [](const IntrinsicsManager&, const llvm::CallInst& ci) {
        return ci.getCalledFunction()->getName() == "borealis_set_property"
               ? function_type::BUILTIN_BOR_SETPROP
               : function_type::UNKNOWN;
    }
};

} // namespace borealis

#pragma clang diagnostic pop

#include "Util/unmacros.h"
