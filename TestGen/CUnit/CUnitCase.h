/*
 * CUnitCase.h
 *
 *  Created on: Apr 7, 2014
 *      Author: gagarski
 */

#ifndef CUNITCASE_H_
#define CUNITCASE_H_

#include "Passes/TestGeneration/FunctionInfoPass.h"
#include "TestGen/TestCase.h"
#include "TestGen/TestSuite.h"

namespace borealis {
namespace util {
class CUnitCaseActivation {
public:
    CUnitCaseActivation() = delete;
    CUnitCaseActivation(const CUnitCaseActivation& activation) = default;
    CUnitCaseActivation(CUnitCaseActivation&& activation) = default;
    CUnitCaseActivation(const TestCase& cs, const TestSuite& suite, int id):
        cs(cs), suite(suite), id(id) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitCaseActivation& activation);
private:
    const TestCase& cs;
    const TestSuite& suite;
    int id;
};

class CUnitCaseDefinition {
public:
    CUnitCaseDefinition() = delete;
    CUnitCaseDefinition(const CUnitCaseDefinition& definition) = default;
    CUnitCaseDefinition(CUnitCaseDefinition&& definition) = default;
    CUnitCaseDefinition(const TestCase& cs, const TestSuite& suite,
            const FunctionInfoPass& fip, int id, const std::vector<Term::Ptr>& oracle) :
            cs(cs), suite(suite), fip(fip), id(id), oracle(oracle) {};
    friend std::ostream& operator<<(std::ostream& os, const CUnitCaseDefinition& definition);
private:
    const TestCase& cs;
    const TestSuite& suite;
    const FunctionInfoPass& fip;
    int id;
    const std::vector<Term::Ptr>& oracle;
};

} /* namespace util */
} /* namespace borealis */

#endif /* CUNITCASE_H_ */
