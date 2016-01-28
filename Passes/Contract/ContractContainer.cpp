//
// Created by kivi on 16.12.15.
//

#include "ContractContainer.h"

namespace borealis {

FunctionIdentifier::Ptr ContractContainer::getFunctionId(llvm::Function* F, std::pair<unsigned int, unsigned int> mem) {
    auto&& newFunc = FunctionIdentifier::Ptr{ new FunctionIdentifier{F, 0, mem} };
    for (auto&& it : data_) {
        if (it.first->equals(newFunc.get())) {
            return it.first;
        }
    }
    return newFunc;
}

}   /* namespace borealis */
