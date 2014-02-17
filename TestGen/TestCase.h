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
public:
    TestCase() = default;
    TestCase(const TestCase& orig) = default;
    explicit TestCase(const std::unordered_map<Term::Ptr, Term::Ptr> & testCase);
    void addArgument(const Term::Ptr arg, const Term::Ptr value);
    const Term::Ptr getValue(const Term::Ptr arg) const;
    virtual ~TestCase();
private:
    std::unordered_map<Term::Ptr, Term::Ptr> testCase;
};

} /* namespace borealis */

#endif	/* TESTCASE_H */

