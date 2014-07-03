/*
 * CUnitMain.h
 *
 *  Created on: Apr 3, 2014
 *      Author: gagarski
 */

#ifndef CUNITMAIN_H_
#define CUNITMAIN_H_
#include <llvm/Module.h>

#include "TestGen/FunctionsInfoData.h"

namespace borealis {
namespace util {

class CUnitMain {
public:
    typedef DataProvider<FunctionsInfoData> FInfoData;
    
    CUnitMain() = delete;
    CUnitMain(const CUnitMain& main) = default;
    CUnitMain(CUnitMain&& main) = default;
    CUnitMain(llvm::Module& module, FInfoData& fInfoData):
        module(module), fInfoData(fInfoData.provide()) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitMain& main);
private:
    llvm::Module& module;
    FunctionsInfoData fInfoData;

};

class CUnitHeader {
public:
    typedef DataProvider<FunctionsInfoData> FInfoData;
    
    CUnitHeader() = delete;
    CUnitHeader(const CUnitHeader& hdr) = default;
    CUnitHeader(CUnitHeader&& hdr) = default;
    CUnitHeader(llvm::Module& module, FInfoData& fInfoData, llvm::StringRef name = llvm::StringRef("test.h")) :
        module(module), fInfoData(fInfoData.provide()), fileName(name) {};

    friend std::ostream& operator<<(std::ostream& os, const CUnitHeader& hdr);
private:
    llvm::Module& module;
    FunctionsInfoData fInfoData;
    llvm::StringRef fileName;
};

} /* namespace util */
} /* namespace borealis */



#endif /* CUNITMAIN_H_ */
