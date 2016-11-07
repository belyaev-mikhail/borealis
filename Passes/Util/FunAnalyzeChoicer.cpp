//
// Created by stepanov on 9/13/16.
//

#include <llvm/Pass.h>

#include "Driver/mpi_driver.h"
#include "Passes/Checker/CheckManager.h"
#include "Passes/Transform/CallGraphChopper.h"
#include "Passes/Util/FunAnalyzeChoicer.h"
#include "Passes/Util/MPIMetricsEval.h"
#include "Util/passes.hpp"

namespace borealis {

void FunAnalyzeChoicer::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AUX<MPIMetricsEval>::addRequiredTransitive(AU);
    AUX<CallGraphChopper>::addRequiredTransitive(AU);
}


bool FunAnalyzeChoicer::runOnModule(llvm::Module& M) {
    CGC = &GetAnalysis<CallGraphChopper>::doit(this);
    CGC->runOnModule(M);
    std::vector<std::pair<llvm::Function*,float>> dif;
    for(auto&& it : M){
        if(!it.isDeclaration()){
            auto&& difficult = GetAnalysis<MPIMetricsEval>::doit(this,it);
            dif.push_back(std::make_pair<llvm::Function*, float>(&it,difficult.getDifficult()));
        }
    }
    std::sort(dif.begin(),dif.end(),[](std::pair<llvm::Function*,float> a, std::pair<llvm::Function*,float> b){return a.second>b.second;});
    int rank, world_size, index;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    std::vector<float>curConsDiff(world_size);
    std::vector<std::vector<llvm::Function*>>funcForConsumers(world_size);
    for(auto&& it : dif){
        auto minIndex = std::min_element(curConsDiff.begin(),curConsDiff.end());
        index = std::distance(curConsDiff.begin(), minIndex);
        funcForConsumers[index].push_back(it.first);
        *minIndex=*minIndex+it.second;
    }
    for(auto&& it : M){
        if(std::find(funcForConsumers[rank].begin(),funcForConsumers[rank].end(),&it) == funcForConsumers[rank].end()){
            it.deleteBody();
        }
    }
    //Stupid distribute
    /*int rank, world_size, index;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int i = 0;
    for(auto&& it : M){
        if(it.isDeclaration() || i%world_size != rank){
            it.deleteBody();
        }
        ++i;
    }*/

    return false;
}


char FunAnalyzeChoicer::ID;
static RegisterPass<FunAnalyzeChoicer>
        X("fun-choicer", "Pass that choose functions that will be analyzed");

} // namespace borealis