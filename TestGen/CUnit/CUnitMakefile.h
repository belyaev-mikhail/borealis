/* 
 * File:   CUnitMakefile.h
 * Author: maxim
 *
 * Created on 11 Апрель 2014 г., 15:55
 */

#ifndef CUNITMAKEFILE_H
#define	CUNITMAKEFILE_H
#include <llvm/Module.h>

namespace borealis {

namespace util {

class CUnitMakefile {
public:
    typedef std::shared_ptr<CUnitMakefile> Ptr;

    CUnitMakefile() = delete;
    CUnitMakefile(const CUnitMakefile& orig) = default;
    CUnitMakefile(CUnitMakefile&& orig) = default;
    CUnitMakefile(llvm::Module& module): module(module) {};
        
    void setBaseDirectory(const std::string& baseDirectory) {
        this->baseDirectory = baseDirectory;
    }
        
    void setName(const std::string& name) {
        this->name = name;
    }
    
    void addSource(const std::string& source) {
        sources.insert(source);
    }
    
    void addTest(const std::string& test) {
        tests.insert(test);
    }
    
    void addOracle(const std::string& oracle) {
        oracles.insert(oracle);
    }

    friend std::ostream& operator<<(std::ostream& os, const CUnitMakefile& makefile);
    
private:
    llvm::Module& module;
    std::string baseDirectory;
    std::string name;
    std::unordered_set<std::string> sources;
    std::unordered_set<std::string> tests;
    std::unordered_set<std::string> oracles;

};

} /* namespace util */

} /* namespace borealis */

#endif	/* CUNITMAKEFILE_H */

