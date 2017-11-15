//
// Created by abdullin on 10/31/17.
//

#ifndef BOREALIS_CONDITIONSPLITTER_H
#define BOREALIS_CONDITIONSPLITTER_H

#include "Interpreter/PredicateState/State.h"
#include "Term/BinaryTerm.h"
#include "Term/CmpTerm.h"

namespace borealis {
namespace absint {
namespace ps {

class ConditionSplitter: public logging::ObjectLevelLogging<ConditionSplitter> {
public:
    using TermMap = std::unordered_map<Term::Ptr, Split, TermHash, TermEqualsWType>;

    ConditionSplitter(Term::Ptr condition, State::Ptr state);

    TermMap apply();

private:

    void visitCmpTerm(const CmpTerm* term);
    void visitBinaryTerm(const BinaryTerm* term);

    Term::Ptr condition_;
    State::Ptr state_;
    TermMap result_;

};

}   // namespace ps
}   // namespace absint
}   // namespace borealis

#endif //BOREALIS_CONDITIONSPLITTER_H
