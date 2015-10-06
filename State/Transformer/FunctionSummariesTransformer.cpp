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
        llvm::iplist<llvm::Argument>& args,const TermMap& TM,
        const ChoiceInfo& ci,const Term::Ptr rv):Base(FN),mapping(TM),choiceInfo(ci),rtv(rv){

    for(auto ar=args.begin(),er=args.end();ar!=er;++ar)
     {
         llvm::Value *x = ar;
         auto&& term = FN.Term->getValueTerm(x);
         for(auto a=mapping.begin(),e=mapping.end();a!=e;++a){
             Term::Ptr tp=a->second;
             for(auto iter=tp->getSubterms().begin(),ite=tp->getSubterms().end();iter!=ite;++iter){
                 errs()<<"ITER="<<*iter<<"\n";
                 errs()<<*iter<<" == "<<"term "<<term<<" ? \n";
                 bool asf=term->equals(iter->get());
                 errs()<<asf<<"\n";
                 if(term->equals(iter->get()) && !isOpaqueTerm(term)){
                     int dist=std::distance(args.begin(),ar);
                     termToArg[term]=dist;
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
        if (auto&& optRef = util::at(mapping, t)) {
            auto&& res = optRef.getUnsafe();
            if (util::contains(arguments, res)) {
                argToTerms[termToArg[res]].insert(t);
                flag = true;
            }
        }
    }

    return flag;
}

    int curPredi=-1;

Predicate::Ptr FunctionSummariesTransformer::transformPredicate(Predicate::Ptr pred) {
    if (pred->getType() == PredicateType::PATH) {
        int k=choiceInfo.size();

        if(curPredi<k-1) {
            ++curPredi;
            errs()<<"xlsize="<<choiceInfo[curPredi].size()<<"\n";
            if(choiceInfo[curPredi].size()==2){
                for (auto &&op :choiceInfo[curPredi][1]->getOperands()) {
                    errs()<<mapping[rtv]<<" and "<<op<<"\n";
                    if(op.get()->equals(mapping[rtv].get()))
                        return nullptr;
                }
            }
            errs() << "pred=" << pred << "\n";
            TermMap m;
            for (auto &&op : pred->getOperands()) {
                errs() << "Op1=" << op << "\n";
                if (checkTerm(op)) {
                    m[op] = op;
                    errs() << "Check op " << op << "\n";
                }
                if (auto &&optRef = util::at(mapping, op)) {
                    auto &&res = optRef.getUnsafe();
                    errs() << "res=" << res << "\n";
                    errs() << checkTerm(res);
                    errs() << "ARGUMENTS:" << arguments << "\n\n\n\n";
                    if (checkTerm(res)) {
                        errs() << "Op=" << op << "\n";
                        m[op] = res;
                    };
                }
            }

            if (not m.empty()) {
                errs() << "REPLACE=" << Predicate::Ptr{pred->replaceOperands(m)} << "\n\n\n";
                return Predicate::Ptr{pred->replaceOperands(m)};
            }
        }
    }

    return nullptr;
    //return pred;
}





bool FunctionSummariesTransformer::isOpaqueTerm(Term::Ptr term) {
    if (llvm::is_one_of<
        OpaqueBoolConstantTerm,
        OpaqueIntConstantTerm,
        OpaqueFloatingConstantTerm,
        OpaqueStringConstantTerm,
        OpaqueNullPtrTerm
    >(term)) {
        return true;
    } else {
        return false;
    }
}





} /*namespace borealis*/

