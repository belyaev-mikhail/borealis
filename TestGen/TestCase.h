/* 
 * File:   TestCase.h
 * Author: maxim
 *
 * Created on 12 Февраль 2014 г., 13:51
 */

#ifndef TESTCASE_H
#define	TESTCASE_H

#include "Term/Term.h"

namespace borealis {

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
    virtual ~TestCase();
private:
    TermMap testCase;
};

} /* namespace borealis */

#endif	/* TESTCASE_H */

