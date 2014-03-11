/*
 * ContractStupidifier.cpp
 *
 *  Created on: Jan 21, 2013
 *      Author: belyaev
 */

#include "State/Transformer/ContractStupidifier.h"

namespace borealis {

struct ContractStupidifier::ContractStupidifierImpl {
    TermFactory::Ptr TF;
    std::vector<Term::Ptr> argTerms;
    Term::Ptr resultTerm;
};

TermFactory& ContractStupidifier::factory() {
    return *pimpl->TF;
}

const std::vector<Term::Ptr>& ContractStupidifier::getArgTerms() const {
    return pimpl->argTerms;
}
Term::Ptr ContractStupidifier::getResultTerm() const {
    return pimpl->resultTerm;
}

ContractStupidifier::ContractStupidifier(
        const std::vector<Term::Ptr>& argTerms,
        Term::Ptr resultTerm,
        FactoryNest FN
):
    Base(FN),
    pimpl(
        new ContractStupidifierImpl {
            FN.Term,
            argTerms,
            resultTerm
        }
    )
{}

ContractStupidifier::~ContractStupidifier() {}

void ContractStupidifier::failWith(const std::string& message) {
    static std::string buf;

    std::ostringstream str;
    str << "Error while processing term: "
        << message;

    buf = str.str();
    throw std::runtime_error(buf.c_str());
}

} // namespace borealis
