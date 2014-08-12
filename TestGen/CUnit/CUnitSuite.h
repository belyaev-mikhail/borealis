/*
 * CUnitSuite.h
 *
 *  Created on: Apr 7, 2014
 *      Author: gagarski
 */

#ifndef CUNITSUITE_H_
#define CUNITSUITE_H_

#include "Passes/TestGeneration/FunctionInfoPass.h"
#include "TestGen/TestStatistics.h"
#include "TestGen/TestSuite.h"

namespace borealis {
namespace util {

class CUnitSuiteActivation {
public:
    CUnitSuiteActivation() = delete;
    CUnitSuiteActivation(const CUnitSuiteActivation& activation) = default;
    CUnitSuiteActivation(CUnitSuiteActivation&& activation) = default;
    CUnitSuiteActivation(TestSuite& suite, const FunctionInfoPass& fip): suite(suite), fip(fip) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitSuiteActivation& activation);
private:
    TestSuite& suite;
    const FunctionInfoPass& fip;

};


class CUnitSuitePrototype {
public:
    CUnitSuitePrototype() = delete;
    CUnitSuitePrototype(const CUnitSuitePrototype& prototype) = default;
    CUnitSuitePrototype(CUnitSuitePrototype&& prototype) = default;
    CUnitSuitePrototype(TestSuite& suite, const FunctionInfoPass& fip,
            const FunctionsInfoData* fInfoData) : suite(suite), fip(fip), fInfoData(fInfoData) {};

    friend std::ostream& operator<<(std::ostream& os, const CUnitSuitePrototype& prototype);
private:
    TestSuite& suite;
    const FunctionInfoPass& fip;
    const FunctionsInfoData* fInfoData;
};

class CUnitSuiteDefinitions {
public:
    CUnitSuiteDefinitions() = delete;
    CUnitSuiteDefinitions(const CUnitSuiteDefinitions& defs) = default;
    CUnitSuiteDefinitions(CUnitSuiteDefinitions&& defs) = default;
    CUnitSuiteDefinitions(const TestSuite& suite, const FunctionInfoPass& fip,
            TestStatistics& ts, const std::vector<Term::Ptr>& oracle):
            suite(suite), fip(fip), ts(ts), oracle(oracle) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitSuiteDefinitions& defs);
private:
    const TestSuite& suite;
    const FunctionInfoPass& fip;
    TestStatistics& ts;
    const std::vector<Term::Ptr>& oracle;
};

} /* namespace util */
} /* namespace borealis */


#endif /* CUNITSUITE_H_ */
