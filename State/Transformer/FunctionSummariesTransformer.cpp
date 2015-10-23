/*
 * FunctionSummariesTransformer.cpp
 *
 *  Created on: 22 июня 2015 г.
 *      Author: danya
 */

#include "FunctionSummariesTransformer.h"


#include "Util/algorithm.hpp"

namespace borealis{

    FunctionSummariesTransformer::FunctionSummariesTransformer(const FactoryNest& FN,
                                                               llvm::iplist<llvm::Argument>& args,const TermMap& TM, const ChoiceInfo& ci,const Term::Ptr rv) :
            Base(FN), mapping(TM), choiceInfo(ci), rtv(rv), curPredi(-1) {

        for (auto ar = args.begin(), er = args.end(); ar != er; ++ar) {
            llvm::Value *x = ar;
            auto&& term = FN.Term->getValueTerm(x);
            for (auto a = mapping.begin(), e = mapping.end(); a != e; ++a) {
                Term::Ptr tp = a->second;
                for(auto iter = tp->getSubterms().begin(), ite = tp->getSubterms().end(); iter != ite; ++iter) {
                    if (term->equals(iter->get()) && not isOpaqueTerm(term)) {
                        int dist=std::distance(args.begin(),ar);
                        termToArg[term] = dist;
                        arguments.insert(term);
                    }
                }
            }
            if (auto&& optRef = util::at(mapping, term)) {
                auto&& res = optRef.getUnsafe();
                arguments.insert(res);
            }

        }

    }

    PredicateState::Ptr FunctionSummariesTransformer::transform(PredicateState::Ptr ps) {
        return Base::transform(ps)
                ->filter([](auto&& p) { return !!p; })
                ->simplify();
    }

    bool FunctionSummariesTransformer::checkTerm(Term::Ptr term) {
        auto&& flag = false;
        for (auto&& t : Term::getFullTermSet(term)) {
            if (util::contains(arguments, t)) {
                argToTerms[termToArg[t]].insert(t);
                flag = true;
            }
        }

        return flag;
    }

    Predicate::Ptr FunctionSummariesTransformer::transformPredicate(Predicate::Ptr pred) {
        if (pred->getType() == PredicateType::PATH) {
            int k=choiceInfo.size();

            if (curPredi < k-1) {
                ++curPredi;
                if (choiceInfo[curPredi].size() == 2) {
                    unsigned int count = 0;
                    for (auto &&op :choiceInfo[curPredi][1]->getOperands()) {
                        if(op.get()->equals(mapping[rtv].get()))
                            return nullptr;
                        ++count;
                        if(count==choiceInfo[curPredi][1]->getNumOperands() && not isOpaqueTerm(op))
                            return nullptr;
                    }
                    TermMap m;
                    for (auto &&op : pred->getOperands()) {
                        if (checkTerm(op)) {
                            m[op] = op;
                        }
                    }

                    if (not m.empty()) {
                        return Predicate::Ptr{pred->replaceOperands(m)};
                    }
                }
            }
        }

        return nullptr;
    }

    bool FunctionSummariesTransformer::isOpaqueTerm(Term::Ptr term) {
        return llvm::is_one_of<
                OpaqueBoolConstantTerm,
                OpaqueIntConstantTerm,
                OpaqueFloatingConstantTerm,
                OpaqueStringConstantTerm,
                OpaqueNullPtrTerm
        >(term);
    }

} /*namespace borealis*/
