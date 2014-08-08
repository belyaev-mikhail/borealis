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

            auto&& args = getArgTerms();
            if(val >= args.size()) failWith("\\" + name + " : arg not found");

            return args[val];
        } else {
            failWith("\\" + name + " : unknown builtin");
        }

        BYE_BYE(Term::Ptr, "Unreachable!");
    }

    Term::Ptr transformOpaqueCall(OpaqueCallTermPtr trm) {
        if(auto builtin = llvm::dyn_cast<OpaqueBuiltinTerm>(trm->getLhv())) {
            auto name = builtin->getVName();

            if (name == "property") {   // XXX Can't handle \\property => kill this term
                return nullptr;
            } else if (name == "bound") {   // XXX Can't handle \\bound => kill this term
                return nullptr;
            } else if (name == "is_valid_ptr") { // XXX not full support for is_valid_ptr
                auto rhv = trm->getRhv();
                if(rhv.size() != 1) {
                    failWith("Illegal \\is_valid_ptr access " +
                            trm->getName() + ": exactly one operand expected");
                }
                auto invalid = factory().getValueTerm(trm->getType(), "0");
                return FN.Term->getCmpTerm(llvm::ConditionType::NEQ,
                                           rhv[0],
                                           invalid);
            } else if (name == "old") { // XXX Can't handle \\old => kill this term
                return nullptr;
            } else {
                failWith("Cannot call " + trm->getName() + ": not supported");
            }
        } else {
            failWith("Cannot call " + trm->getName() + ": only builtins can be called in this way");
        }
        BYE_BYE(Term::Ptr, "Unreachable!");
    }

    Term::Ptr transformBinaryTerm(BinaryTermPtr trm) {
        switch(trm->getOpcode()) {
        case llvm::ArithType::LSHR:
            return FN.Term->getBinaryTerm(llvm::ArithType::ASHR, trm->getLhv(), trm->getRhv());
        default:
            return trm;
        }
        return trm;
    }

    Term::Ptr transformCmpTerm(CmpTermPtr trm) {
        switch(trm->getOpcode()) {
        case llvm::ConditionType::TRUE:
            return FN.Term->getBooleanTerm(true);
        case llvm::ConditionType::FALSE:
            return FN.Term->getBooleanTerm(false);
        default:
            return FN.Term->getCmpTerm(
                llvm::forceSigned(trm->getOpcode()),
                trm->getLhv(),
                trm->getRhv()
            );
        }
        return trm;
    }

    Term::Ptr transformCastTerm(CastTermPtr trm) {
        return trm->getRhv();
    }
};


} /* namespace borealis */

#include "Util/unmacros.h"

#endif /* CONTRACTSTUPIDIFIER_H_ */
