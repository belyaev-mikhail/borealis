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
    const FunctionInfo* fi;
    Term::Ptr resultTerm;
};

TermFactory& ContractStupidifier::factory() {
    return *pimpl->TF;
}

const FunctionInfo* ContractStupidifier::getFunctionInfo() const {
    return pimpl->fi;
}
Term::Ptr ContractStupidifier::getResultTerm() const {
    return pimpl->resultTerm;
}

ContractStupidifier::ContractStupidifier(
        const FunctionInfo* fi,
        Term::Ptr resultTerm,
        FactoryNest FN
):
    Base(FN),
    pimpl(
        new ContractStupidifierImpl {
            FN.Term,
            fi,
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
