/*
 * CUnitSuite.h
 *
 *  Created on: Apr 7, 2014
 *      Author: gagarski
 */

#ifndef CUNITSUITE_H_
#define CUNITSUITE_H_

#include "TestGen/TestSuite.h"

namespace borealis {
namespace util {

class CUnitSuiteActivation {
public:
    CUnitSuiteActivation() = delete;
    CUnitSuiteActivation(const CUnitSuiteActivation& activation) = default;
    CUnitSuiteActivation(CUnitSuiteActivation&& activation) = default;
    CUnitSuiteActivation(TestSuite& suite): suite(suite) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitSuiteActivation& activation);
private:
    TestSuite& suite;

};


class CUnitSuitePrototype {
public:
    CUnitSuitePrototype() = delete;
    CUnitSuitePrototype(const CUnitSuitePrototype& prototype) = default;
    CUnitSuitePrototype(CUnitSuitePrototype&& prototype) = default;
    CUnitSuitePrototype(TestSuite& suite, const MetaInfoTracker& mit,
            const PrototypesInfo* prototypes) : suite(suite), mit(mit), prototypes(prototypes) {};

    friend std::ostream& operator<<(std::ostream& os, const CUnitSuitePrototype& prototype);
private:
    TestSuite& suite;
    const MetaInfoTracker& mit;
    const PrototypesInfo* prototypes;
};

class CUnitSuiteDefinitions {
public:
    CUnitSuiteDefinitions() = delete;
    CUnitSuiteDefinitions(const CUnitSuiteDefinitions& defs) = default;
    CUnitSuiteDefinitions(CUnitSuiteDefinitions&& defs) = default;
    CUnitSuiteDefinitions(const TestSuite& suite, const FactoryNest& fn,
            const MetaInfoTracker& mit, const std::vector<Term::Ptr>& oracle):
            suite(suite), fn(fn), mit(mit), oracle(oracle) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitSuiteDefinitions& defs);
private:
    const TestSuite& suite;
    const FactoryNest& fn;
    const MetaInfoTracker& mit;
    const std::vector<Term::Ptr>& oracle;
};

} /* namespace util */
} /* namespace borealis */


#endif /* CUNITSUITE_H_ */
