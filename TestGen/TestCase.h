/* 
 * File:   TestCase.h
 * Author: maxim
 *
 * Created on 12 Февраль 2014 г., 13:51
 */

#ifndef TESTCASE_H
#define	TESTCASE_H

#include "Factory/Nest.h"
#include "Passes/Tracker/MetaInfoTracker.h"
#include "Term/Term.h"

namespace borealis {

class TestSuite;
    
class TestCase {
private:
    typedef std::unordered_map<Term::Ptr, Term::Ptr, std::hash<const Term::Ptr>, termPtrEqual> TermMap; 
public:
    typedef std::shared_ptr<TestCase> Ptr;

    TestCase() = default;
    TestCase(const TestCase & orig) = default;
    TestCase(TestCase && orig) = default;
    explicit TestCase(const TermMap & testCase);
    
    void addArgument(const Term::Ptr arg, const Term::Ptr value);
    const Term::Ptr getValue(const Term::Ptr arg) const;
    
    void generateTest(std::ostream & outStream, const llvm::Function * F,  FactoryNest fn, MetaInfoTracker * mit, int id);
    void activateTest(std::ostream & outStream, const TestSuite & suite) const;

    std::string getTestName(const llvm::Function * function) const;
    std::string getTestName(llvm::StringRef functionName) const;
private:
    static llvm::StringRef getArgName(const llvm::DIType * type);
    
    TermMap testCase;
    int id;
};

} /* namespace borealis */

#endif	/* TESTCASE_H */

