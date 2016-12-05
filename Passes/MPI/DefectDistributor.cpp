//
// Created by stepanov on 11/28/16.
//

#include "DefectDistributor.h"
#include "Util/passes.hpp"

#include <fstream>
#include <llvm/Analysis/LoopPass.h>

namespace borealis{

void DefectDistributor::getAnalysisUsage(llvm::AnalysisUsage& AU) const {
    AU.setPreservesAll();
    AUX<DefectManager>::addRequiredTransitive(AU);
}

void DefectDistributor::receiveDefects(SimpleT& reciever){
    auto&& recArray = driver.receiveBytesArray().getData();
    std::istringstream in(recArray);
    auto&& recDefects = util::read_as_json<SimpleT>(in);
    reciever.first.insert(recDefects->first.begin(), recDefects->first.end());
    reciever.second.insert(recDefects->second.begin(), recDefects->second.end());
}

void DefectDistributor::sendDefects(SimpleT& defects, mpi::Rank recRank){
    std::stringstream json;
    util::write_as_json(json, defects);
    driver.sendBytesArray(recRank, {json.str(), mpi::Tag::DataTag::BYTEARRAY});
}

void DefectDistributor::dumpDefectsInFile(SimpleT& defects){
    std::ofstream out{jsonFileName};
    util::write_as_json(out, std::make_pair(std::move(defects.first), std::move(defects.second)));
}

bool DefectDistributor::runOnModule(llvm::Module&){
    DM = &GetAnalysis<DefectManager>::doit(this);
    const int localRoot = driver.getGlobalRankOfLocalRoot();
    const int nodeSize = driver.getNodeSize();
    const int size = driver.getSize();
    const int numOfNodes = size/nodeSize;

    if(!driver.isLocalRoot()){
        auto&& defects = DM->getAllData();
        sendDefects(defects, localRoot);
    }
    else{
        auto&& localRootDefects = DM->getAllData();
        for(auto&& i = 0; i < driver.getNodeSize()-1; ++i){
            receiveDefects(localRootDefects);
        }
        if(not(driver.isGlobalRoot())){
            sendDefects(localRootDefects, mpi::Rank::ROOT);
            //receive merged defects and dump it's to file
            SimpleT mergedDefects;
            receiveDefects(mergedDefects);
            dumpDefectsInFile(mergedDefects);
        }
        else{
            auto&& rootDefects = localRootDefects;
            auto&& pastDefects = DM->getAllPastData();
            rootDefects.first.insert(pastDefects.first.begin(), pastDefects.first.end());
            rootDefects.second.insert(pastDefects.second.begin(), pastDefects.second.end());
            for(auto&& i = 0; i < numOfNodes-1; ++i) {
                receiveDefects(rootDefects);
            }
            //erase
            for(auto&& d:rootDefects.first) rootDefects.second.erase(d);
            //dump into file in send merged defects to others local roots
            dumpDefectsInFile(rootDefects);
            for(auto i = nodeSize; i < size; i+=nodeSize){
                sendDefects(rootDefects, i);
            }
        }
    }
    return false;
}

char DefectDistributor::ID;
static RegisterPass<DefectDistributor>
        X("defect-distributor", "Pass that merge and distribute defects on all nodes in MPI program");

} // namespace Borealis