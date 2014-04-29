/*
 * CUnitMain.h
 *
 *  Created on: Apr 3, 2014
 *      Author: gagarski
 */

#ifndef CUNITMAIN_H_
#define CUNITMAIN_H_
#include <llvm/Module.h>

namespace borealis {
namespace util {

class CUnitMain {
public:
    CUnitMain() = delete;
    CUnitMain(const CUnitMain& main) = default;
    CUnitMain(CUnitMain&& main) = default;
    CUnitMain(llvm::Module& module): module(module) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitMain& main);
private:
    llvm::Module& module;

};

class CUnitHeader {
public:
    CUnitHeader() = delete;
    CUnitHeader(const CUnitHeader& hdr) = default;
    CUnitHeader(CUnitHeader&& hdr) = default;
    CUnitHeader(llvm::Module& module, llvm::StringRef name = llvm::StringRef("test.h")) :
        module(module), fileName(name) {};

    friend std::ostream& operator<<(std::ostream& os, const CUnitHeader& hdr);
private:
    llvm::Module& module;
    llvm::StringRef fileName;
};

} /* namespace util */
} /* namespace borealis */



#endif /* CUNITMAIN_H_ */
