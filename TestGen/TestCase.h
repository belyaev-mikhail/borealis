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

#include "Util/hash.hpp"

namespace borealis {

class TestSuite;
    
class TestCase {
private:
    typedef std::unordered_map<Term::Ptr, Term::Ptr, std::hash<const Term::Ptr>, termPtrEqual> TermMap; 
    typedef TermMap::const_iterator const_iterator;
public:
    typedef std::shared_ptr<TestCase> Ptr;

    TestCase() = default;
    TestCase(const TestCase & orig) = default;
    TestCase(TestCase && orig) = default;
    explicit TestCase(const TermMap & testCase);
    
    void addArgument(const Term::Ptr arg, const Term::Ptr value);
    const Term::Ptr getValue(const Term::Ptr arg) const;
    
    void generateTest(std::ostream & outStream, const llvm::Function * F,
        FactoryNest fn, MetaInfoTracker * mit, int id, const std::vector<Term::Ptr>& oracle,
        std::string resultName) const;
    void activateTest(std::ostream & outStream, const TestSuite & suite, int id) const;

    std::string getTestName(const llvm::Function * function, int id) const;
    std::string getTestName(llvm::StringRef functionName, int id) const;

    friend bool operator==(const TestCase& lhv, const TestCase& rhv) {
        return lhv.testCase.size() == rhv.testCase.size()
            && util::viewContainer(lhv.testCase).all_of(
                    [&rhv](const std::pair<Term::Ptr,Term::Ptr>& kv) -> bool {
                        for(auto&& rv : util::at(rhv.testCase, kv.first)) return *rv == *kv.second;
                        return false;
                    }
               );
    }

    size_t hashCode() const {
        util::hash::defaultHasher hasher;
        size_t hash = 0;
        // FIXME: this hash sucks, can't we do better?
        for(auto&& kv : testCase) {
            hash ^= hasher(kv.first, kv.second);
        }
        return hash;
    }
    const_iterator begin() const { return testCase.begin(); }
    const_iterator end() const { return testCase.end(); }

private:
    TermMap testCase;
};

namespace util {
////////////////////////////////////////////////////////////////////////////////
// Json
////////////////////////////////////////////////////////////////////////////////
template<>
struct json_traits<TestCase> {
    typedef std::unique_ptr<TestCase> optional_ptr_t;

    static Json::Value toJson(const TestCase& val) {
        Json::Value args;
        for (auto arg : val) {
            if (arg.first != nullptr)
                args[arg.first->getName()] = arg.second->getName();
        }
        return args;
    }
};
} // namespace util

} /* namespace borealis */

namespace std {

template<>
struct hash<borealis::TestCase> {
    size_t operator()(const borealis::TestCase& tc) const noexcept {
        return tc.hashCode();
    }
};

template<>
struct hash<const borealis::TestCase> {
    size_t operator()(const borealis::TestCase& tc) const noexcept {
        return tc.hashCode();
    }
};

} /* namespace std */

#endif	/* TESTCASE_H */

