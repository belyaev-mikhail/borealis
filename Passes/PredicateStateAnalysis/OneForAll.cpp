/*
 * OneForAll.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: ice-phoenix
 */

#include <llvm/Support/CFG.h>

#include "Logging/tracer.hpp"
#include "Passes/PredicateAnalysis/PredicateAnalysis.def"
#include "Passes/PredicateStateAnalysis/OneForAll.h"
#include "Passes/SlotTrackerPass.h"
#include "State/PredicateStateBuilder.h"
#include "State/Transformer/CallSiteInitializer.h"
#include "Util/graph.h"
#include "Util/util.h"

#include "Util/macros.h"

namespace borealis {

OneForAll::OneForAll() : ProxyFunctionPass(ID) {}
OneForAll::OneForAll(llvm::Pass* pass) : ProxyFunctionPass(ID, pass) {}

void OneForAll::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();

    AUX<llvm::DominatorTree>::addRequired(AU);
    AUX<FunctionManager>::addRequiredTransitive(AU);
    AUX<SlotTrackerPass>::addRequiredTransitive(AU);

#define HANDLE_ANALYSIS(CLASS) \
    AUX<CLASS>::addRequiredTransitive(AU);
#include "Passes/PredicateAnalysis/PredicateAnalysis.def"
}

bool OneForAll::runOnFunction(llvm::Function& F) {
    init();

    FM = &GetAnalysis< FunctionManager >::doit(this, F);
    DT = &GetAnalysis< llvm::DominatorTree >::doit(this, F);

    auto* ST = GetAnalysis< SlotTrackerPass >::doit(this, F).getSlotTracker(F);
    PF = PredicateFactory::get(ST);
    TF = TermFactory::get(ST);

    PSF = PredicateStateFactory::get();

#define HANDLE_ANALYSIS(CLASS) \
    PA.push_back(static_cast<AbstractPredicateAnalysis*>(&GetAnalysis<CLASS>::doit(this, F)));
#include "Passes/PredicateAnalysis/PredicateAnalysis.def"

    // Register globals in our predicate states
    auto& globalList = F.getParent()->getGlobalList();

    std::vector<Term::Ptr> globals;
    globals.reserve(globalList.size());
    for (auto& g : globalList) {
        globals.push_back(TF->getValueTerm(&g));
    }
    Predicate::Ptr gPredicate = PF->getGlobalsPredicate(globals);

    // Register REQUIRES from annotations
    PredicateState::Ptr requires = FM->get(&F)->filterByTypes({ PredicateType::REQUIRES });

    PredicateState::Ptr initialState = (PSF * gPredicate + requires)();

    // Register arguments as visited values
    for (const auto& arg : F.getArgumentList()) {
        initialState = initialState << arg;
    }

    // Initial state goes in nullptr
    basicBlockStates[nullptr] = initialState;

    // Process basic blocks in topological order
    TopologicalSorter::Result ordered = TopologicalSorter().doit(F);
    ASSERT(!ordered.empty(),
           "No topological order for: " + F.getName().str());
    ASSERT(ordered.getUnsafe().size() == F.getBasicBlockList().size(),
           "Topological order does not include all basic blocks for: " + F.getName().str());

    dbgs() << "Topological sorting for: " << F.getName() << endl;
    for (auto* BB : ordered.getUnsafe()) {
        dbgs() << valueSummary(BB) << endl;
    }
    dbgs() << "End of topological sorting for: " << F.getName() << endl;

    for (auto* BB : ordered.getUnsafe()) {
        processBasicBlock(BB);
    }

    return false;
}

void OneForAll::print(llvm::raw_ostream&, const llvm::Module*) const {
    infos() << "Predicate state analysis results" << endl;
    for (const auto& e : instructionStates) {
        infos() << *e.first << endl
                << e.second << endl;
    }
    infos() << "End of predicate state analysis results" << endl;
}

////////////////////////////////////////////////////////////////////////////////

void OneForAll::init() {
    AbstractPredicateStateAnalysis::init();
    basicBlockStates.clear();
}

void OneForAll::processBasicBlock(llvm::BasicBlock* BB) {
    using namespace llvm;
    using borealis::util::view;

    auto inState = BBM(BB);

    if (inState == nullptr) return;
    if (PredicateStateAnalysis::CheckUnreachable() && inState->isUnreachable()) return;

    for (const auto& I : view(BB->begin(), BB->end())) {

        auto instructionState = (PSF * inState + PM(&I) << I)();
        instructionStates[&I] = instructionState;

        // Add ensures and summary *after* the CallInst has been processed
        if (isa<CallInst>(I)) {
            auto& CI = cast<CallInst>(I);

            auto callState = FM->get(CI, PF.get(), TF.get());
            CallSiteInitializer csi(CI, TF.get());

            auto csiCallState = callState
                ->filterByTypes(
                    { PredicateType::ENSURES, PredicateType::STATE }
                )->map(
                    [&csi](Predicate::Ptr p) { return csi.transform(p); }
                );

            instructionState = (PSF * instructionState + csiCallState)();
        }

        inState = instructionState;
    }

    basicBlockStates[BB] = inState;
}

////////////////////////////////////////////////////////////////////////////////

PredicateState::Ptr OneForAll::BBM(llvm::BasicBlock* BB) {
    using namespace llvm;
    using borealis::util::containsKey;
    using borealis::util::view;

    TRACE_UP("psa::bbm", valueSummary(BB));

    const auto* idom = (*DT)[BB]->getIDom();
    // Function entry block does not have an idom
    if (!idom) {
        return basicBlockStates.at(nullptr);
    }

    const auto* idomBB = idom->getBlock();
    // idom is unreachable
    if (!containsKey(basicBlockStates, idomBB)) {
        return nullptr;
    }

    auto base = basicBlockStates.at(idom->getBlock());
    std::vector<PredicateState::Ptr> choices;

    for (const auto* predBB : view(pred_begin(BB), pred_end(BB))) {
        // predecessor is unreachable
        if (!containsKey(basicBlockStates, predBB)) continue;

        auto stateBuilder = PSF * basicBlockStates.at(predBB);

        // Adding path predicate from predBB
        stateBuilder += TPM({predBB->getTerminator(), BB});

        // Adding PHI predicates from predBB
        for (auto it = BB->begin(); isa<PHINode>(it); ++it) {
            const PHINode* phi = cast<PHINode>(it);
            if (phi->getBasicBlockIndex(predBB) != -1) {
                stateBuilder += PPM({predBB, phi}) << phi;
            }
        }

        auto inState = stateBuilder();

        auto slice = inState->sliceOn(base);
        ASSERT(slice != nullptr, "Could not slice state on its predecessor");

        choices.push_back(slice);
    }

    TRACE_DOWN("psa::bbm", valueSummary(BB));

    if (choices.empty())
        // All predecessors are unreachable, and we too are as such...
        return nullptr;
    else
        return (
            PSF *
            base +
            PSF->Choice(choices)
        )();
}

PredicateState::Ptr OneForAll::PM(const llvm::Instruction* I) {
    using borealis::util::containsKey;

    PredicateState::Ptr res = PSF->Basic();

    for (AbstractPredicateAnalysis* APA : PA) {
        auto& map = APA->getPredicateMap();
        if (containsKey(map, I)) {
            res = res + map.at(I);
        }
    }

    return res;
}

PredicateState::Ptr OneForAll::PPM(PhiBranch key) {
    using borealis::util::containsKey;

    PredicateState::Ptr res = PSF->Basic();

    for (AbstractPredicateAnalysis* APA : PA) {
        auto& map = APA->getPhiPredicateMap();
        if (containsKey(map, key)) {
            res = res + map.at(key);
        }
    }

    return res;
}

PredicateState::Ptr OneForAll::TPM(TerminatorBranch key) {
    using borealis::util::containsKey;

    PredicateState::Ptr res = PSF->Basic();

    for (AbstractPredicateAnalysis* APA : PA) {
        auto& map = APA->getTerminatorPredicateMap();
        if (containsKey(map, key)) {
            res = res + map.at(key);
        }
    }

    return res;
}

////////////////////////////////////////////////////////////////////////////////

char OneForAll::ID;
static RegisterPass<OneForAll>
X("one-for-all", "One-for-all predicate state analysis");

} /* namespace borealis */

#include "Util/unmacros.h"
