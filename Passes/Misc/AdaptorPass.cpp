/*
 * AdaptorPass.cpp
 *
 *  Created on: Aug 22, 2012
 *      Author: ice-phoenix
 */

#include <llvm/IR/Constants.h>
#include <llvm/IR/InstVisitor.h>

#include "Codegen/intrinsics_manager.h"
#include "Config/config.h"
#include "Passes/Misc/AdaptorPass.h"
#include "Util/functional.hpp"

namespace borealis {

namespace {

using namespace borealis::config;
MultiConfigEntry asserts{"adapt", "assert"};
MultiConfigEntry assumes{"adapt", "assume"};
MultiConfigEntry nondets{"adapt", "nondet"};
MultiConfigEntry errors {"adapt", "error"};
MultiConfigEntry labels {"adapt", "error-label"};
BoolConfigEntry undefinedDefaultsToUnknown{"adapt", "break-undefs"};

llvm::Function* getBorealisBuiltin(
    function_type ft,
    const std::vector<llvm::Type*>& argTypes,
    llvm::Module& M
) {
    auto&& intrinsic_manager = IntrinsicsManager::getInstance();

    auto&& retType = llvm::Type::getVoidTy(M.getContext());

    auto* cur = M.getFunction(
        intrinsic_manager.getFuncName(ft, "")
    );

    if (not cur) {
        // FIXME: name clashes still possible due to linking, research that
        auto&& ty = llvm::FunctionType::get(
            retType,
            argTypes,
            false
        );

        cur = intrinsic_manager.createIntrinsic(
            ft,
            "",
            ty,
            &M
        );
    }

    return cur;
}

llvm::Value* mkBorealisBuiltin(
    function_type ft,
    const std::vector<llvm::Type*>& argTypes,
    llvm::Module& M,
    llvm::CallInst* existingCall
) {
    using llvm::dyn_cast;

    auto&& cur = getBorealisBuiltin(ft, argTypes, M);

    std::vector<llvm::Value*> realArgs;

    for (auto&& p : util::viewContainer(existingCall->arg_operands()) ^ util::viewContainer(argTypes)) {
        auto&& expectedType = p.second;
        auto&& realValue = p.first;

        auto&& castOpCode = llvm::CastInst::getCastOpcode(
            realValue,
            false,
            expectedType,
            false
        );

        realArgs.push_back(
            llvm::CastInst::Create(
                castOpCode,
                realValue,
                expectedType,
                "borealis.builtin_arg",
                existingCall
            )
        );
    }

    auto* newCall = llvm::CallInst::Create(
        cur,
        realArgs,
        "",
        existingCall
    );
    newCall->setDebugLoc(existingCall->getDebugLoc());
    newCall->setMetadata("dbg", existingCall->getMetadata("dbg"));

    if (existingCall->getType() != newCall->getType()) {
        existingCall->replaceAllUsesWith(llvm::UndefValue::get(existingCall->getType()));
    } else {
        existingCall->replaceAllUsesWith(newCall);
    }
    existingCall->eraseFromParent();
    return newCall;
}

llvm::Value* mkBorealisAssert(
    llvm::Module& M,
    llvm::CallInst* existingCall) {
    return mkBorealisBuiltin(
        function_type::BUILTIN_BOR_ASSERT,
        { llvm::Type::getInt32Ty(M.getContext()) },
        M,
        existingCall);
}

llvm::Value* mkBorealisAssume(
    llvm::Module& M,
    llvm::CallInst* existingCall) {
    return mkBorealisBuiltin(
        function_type::BUILTIN_BOR_ASSUME,
        { llvm::Type::getInt32Ty(M.getContext()) },
        M,
        existingCall);
}

llvm::Value* mkBorealisError(
    llvm::Module& M,
    llvm::CallInst* existingCall) {
    return mkBorealisBuiltin(
        function_type::ACTION_DEFECT,
        {},
        M,
        existingCall);
}

llvm::Value* mkBorealisNonDet(llvm::Module& M, llvm::Type* type, llvm::Instruction* insertBefore) {
    auto&& intrinsic_manager = IntrinsicsManager::getInstance();
    auto&& func = intrinsic_manager.createIntrinsic(
        function_type::INTRINSIC_NONDET,
        util::toString(*type),
        llvm::FunctionType::get(type, false),
        &M
    );

    return llvm::CallInst::Create(func, "", insertBefore);
}

class CallVisitor : public llvm::InstVisitor<CallVisitor> {
public:
    // XXX: still buggy, unreachable propagation breaks everything to pieces
    void visitBasicBlock(llvm::BasicBlock& BB) {
        using namespace llvm;
        auto&& M = *BB.getParent()->getParent();

        // define all undefined variables
        if (undefinedDefaultsToUnknown.get(false)) {
            using namespace borealis::util;
            auto&& allocas =
                viewContainer(BB)
               .map(ops::take_pointer)
               .filter(llvm::isaer<AllocaInst>{})
               .map(llvm::caster<AllocaInst>{})
               .toHashSet();

            for (auto&& alloca : viewContainer(allocas)) {
                if (alloca->getAllocatedType()->isSingleValueType()) {
                    auto&& nonDet = mkBorealisNonDet(M, alloca->getAllocatedType(), alloca);
                    (new StoreInst{ nonDet, alloca })->insertAfter(alloca);
                }
            }
        }

    }

    void visitCall(llvm::CallInst& I) {
        using namespace llvm;

        static auto assert = util::viewContainer(asserts.get()).toHashSet();
        static auto assume = util::viewContainer(assumes.get()).toHashSet();
        static auto error  = util::viewContainer(errors.get()).toHashSet();
        static auto nondet = util::viewContainer(nondets.get()).toHashSet();

        auto&& calledFunc = I.getCalledFunction();

        if (not calledFunc) return;
        if (not calledFunc->hasName()) return;

        auto&& M = *I.getParent()->getParent()->getParent();
        auto&& calledFuncName = calledFunc->getName();

        if (assert.count(calledFuncName)) {
            mkBorealisAssert(M, &I);
        } else if (assume.count(calledFuncName)) {
            mkBorealisAssume(M, &I);
        } else if (error.count(calledFuncName)) {
            mkBorealisError(M, &I);
        } else if (nondet.count(calledFuncName)) {
            auto&& nonDetValue = mkBorealisNonDet(M, I.getType(), &I);
            I.replaceAllUsesWith(nonDetValue);
        }
    }

};

} /* namespace */

////////////////////////////////////////////////////////////////////////////////

AdaptorPass::AdaptorPass() : ModulePass(ID) {}

void AdaptorPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesCFG();
}

bool AdaptorPass::runOnModule(llvm::Module& M) {
    CallVisitor{}.visit(M);
    return false;
}

AdaptorPass::~AdaptorPass() {}

char AdaptorPass::ID;
static RegisterPass<AdaptorPass>
X("adaptor", "Adapt the borealis assertions API to custom functions or labels");

} /* namespace borealis */
