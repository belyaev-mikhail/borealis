/*
 * PredicateAnalysis.cpp
 *
 *  Created on: Aug 31, 2012
 *      Author: ice-phoenix
 */

#include "PredicateAnalysis.h"
#include "SlotTrackerPass.h"

#include <llvm/BasicBlock.h>
#include <llvm/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "Predicate/Predicate.h"
#include "Predicate/LoadPredicate.h"
#include "Predicate/StorePredicate.h"
#include "Predicate/ICmpPredicate.h"
#include "Predicate/BooleanPredicate.h"
#include "Predicate/GEPPredicate.h"
#include "Predicate/EqualityPredicate.h"

namespace borealis {

using util::for_each;
using util::toString;

PredicateAnalysis::PredicateAnalysis() : llvm::FunctionPass(ID) {
	// TODO
}

void PredicateAnalysis::getAnalysisUsage(llvm::AnalysisUsage& Info) const {
	using namespace::llvm;

	Info.setPreservesAll();
	Info.addRequiredTransitive<SlotTrackerPass>();
	Info.addRequiredTransitive<TargetData>();
}

bool PredicateAnalysis::runOnFunction(llvm::Function& F) {
	using namespace::std;
	using namespace::llvm;

	predicateMap.clear();

	TD = &getAnalysis<TargetData>();
	st = getAnalysis<SlotTrackerPass>().getSlotTracker(F);

	for_each(F, [this](const BasicBlock& BB){
		for_each(BB, [this](const Instruction& I){
			processInst(I);
		});
	});

	return false;
}

void PredicateAnalysis::processInst(const llvm::Instruction& I) {
	using namespace::llvm;

	if (isa<LoadInst>(I))
		{ process(cast<LoadInst>(I)); }

	else if (isa<StoreInst>(I))
		{ process(cast<StoreInst>(I)); }

	else if (isa<ICmpInst>(I))
		{ process(cast<ICmpInst>(I)); }

	else if (isa<BranchInst>(I))
		{ process(cast<BranchInst>(I)); }

    else if (isa<GetElementPtrInst>(I))
        { process(cast<GetElementPtrInst>(I)); }

    else if (isa<SExtInst>(I))
        { process(cast<SExtInst>(I)); }
}

void PredicateAnalysis::process(const llvm::LoadInst& I) {
	using namespace::std;
	using namespace::llvm;

	const LoadInst* lhv = &I;
	const Value* rhv = I.getPointerOperand();

	predicateMap[&I] = new LoadPredicate(lhv, rhv, st);
}

void PredicateAnalysis::process(const llvm::StoreInst& I) {
	using namespace::std;
	using namespace::llvm;

	const Value* lhv = I.getPointerOperand();
	const Value* rhv = I.getValueOperand();

	predicateMap[&I] = new StorePredicate(lhv, rhv, st);
}

void PredicateAnalysis::process(const llvm::ICmpInst& I) {
	using namespace::std;
	using namespace::llvm;

	const Value* op1 = I.getOperand(0);
	const Value* op2 = I.getOperand(1);
	int pred = I.getPredicate();

	predicateMap[&I] = new ICmpPredicate(&I, op1, op2, pred, st);
}

void PredicateAnalysis::process(const llvm::BranchInst& I) {
	using namespace::std;
	using namespace::llvm;

	if (I.isUnconditional()) return;

	const Value* cond = I.getCondition();
	const BasicBlock* trueSucc = I.getSuccessor(0);
	const BasicBlock* falseSucc = I.getSuccessor(1);

	terminatorPredicateMap[make_pair(&I, trueSucc)] =
			new BooleanPredicate(PredicateType::PATH, cond, true, st);
	terminatorPredicateMap[make_pair(&I, falseSucc)] =
			new BooleanPredicate(PredicateType::PATH, cond, false, st);
}

void PredicateAnalysis::process(const llvm::GetElementPtrInst& I) {
    using namespace::std;
    using namespace::llvm;

    Type* type = I.getPointerOperandType();

    vector< pair<const Value*, uint64_t> > shifts;
    for (auto it = I.idx_begin(); it != I.idx_end(); ++it) {
        const Value* v = *it;
        const uint64_t size = TD->getTypeAllocSize(type);
        shifts.push_back(make_pair(v, size));

        if (type->isArrayTy()) type = type->getArrayElementType();
        else if (type->isStructTy()) {
            if (!isa<ConstantInt>(v)) {
                errs() << "ACHTUNG!!!" << endl; // TODO: Sayonara
                return;
            }
            auto fieldIdx = cast<ConstantInt>(v)->getZExtValue();
            type = type->getStructElementType(fieldIdx);
        }
    }

    const Value* lhv = &I;
    const Value* rhv = I.getPointerOperand();

    predicateMap[&I] = new GEPPredicate(lhv, rhv, shifts, st);
}

void PredicateAnalysis::process(const llvm::SExtInst& I) {
    using namespace::std;
    using namespace::llvm;

    const Value* lhv = &I;
    const Value* rhv = I.getOperand(0);

    predicateMap[&I] = new EqualityPredicate(lhv, rhv, st);
}

PredicateAnalysis::~PredicateAnalysis() {
	for_each(predicateMap, [this](const PredicateMapEntry& entry){
		delete entry.second;
	});

	for_each(terminatorPredicateMap, [this](const TerminatorPredicateMapEntry& entry){
		delete entry.second;
	});
}

} /* namespace borealis */

char borealis::PredicateAnalysis::ID;
static llvm::RegisterPass<borealis::PredicateAnalysis>
X("predicate", "Instruction predicate analysis", false, false);
