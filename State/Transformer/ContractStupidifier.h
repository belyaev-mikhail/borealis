/*
 * ContractStupidifier.h
 *
 *  Created on: Jan 21, 2013
 *      Author: belyaev
 */

#ifndef CONTRACTSTUPIDIFIER_H_
#define CONTRACTSTUPIDIFIER_H_

#include <memory>
#include <sstream>

#include "Annotation/Annotation.def"
#include "Passes/Tracker/MetaInfoTracker.h"
#include "State/Transformer/Transformer.hpp"
#include "Term/NameContext.h"
#include "Term/TermBuilder.h"
#include "Util/util.h"

#include "Util/macros.h"

namespace borealis {

class ContractStupidifier : public borealis::Transformer<ContractStupidifier> {

    typedef borealis::Transformer<ContractStupidifier> Base;

    struct ContractStupidifierImpl;
    std::unique_ptr<ContractStupidifierImpl> pimpl;

    TermFactory& factory();
    const std::vector<Term::Ptr>& getArgTerms() const;
    Term::Ptr getResultTerm() const;
public:

    ContractStupidifier(
            const std::vector<Term::Ptr>& argTerms,
            Term::Ptr resultTerm,
            FactoryNest FN
    );
    ~ContractStupidifier();

    void failWith(const std::string& message);
    inline void failWith(llvm::Twine twine) {
        failWith(twine.str());
    }
    // resolving ambiguity
    inline void failWith(const char* r) {
        failWith(std::string(r));
    }

    // FIXME: handle builtin calls somehow

    Term::Ptr transformOpaqueBuiltinTerm(OpaqueBuiltinTermPtr trm) {
        const llvm::StringRef name = trm->getVName();

        if (name == "result") {
            return getResultTerm();
        } else if (name == "null" || name == "nullptr") {
            return factory().getValueTerm(trm->getType(), "0");
        } else if (name == "invalid" || name == "invalidptr") {
            return factory().getValueTerm(trm->getType(), "0");
        } else if (name.startswith("arg")) {
            std::istringstream ist(name.drop_front(3).str());
            unsigned val = 0U;
            ist >> val;

            //if(!ist) failWith("\\" + name + " : unknown builtin");

            return getArgTerms()[val];
        } else {
            failWith("\\" + name + " : unknown builtin");
        }

        BYE_BYE(Term::Ptr, "Unreachable!");
    }
};


} /* namespace borealis */

#include "Util/unmacros.h"

#endif /* CONTRACTSTUPIDIFIER_H_ */
