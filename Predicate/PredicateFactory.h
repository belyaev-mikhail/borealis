/*
 * PredicateFactory.h
 *
 *  Created on: Sep 25, 2012
 *      Author: ice-phoenix
 */

#ifndef PREDICATEFACTORY_H_
#define PREDICATEFACTORY_H_

#include <memory>

#include "Predicate/Predicate.def"
#include "Predicate/Predicate.h"

namespace borealis {

class PredicateFactory {

public:

    typedef std::shared_ptr<PredicateFactory> Ptr;

    Predicate::Ptr getLoadPredicate(
            Term::Ptr lhv,
            Term::Ptr loadTerm) {
        return getEqualityPredicate(lhv, loadTerm);
    }

    Predicate::Ptr getStorePredicate(
            Term::Ptr lhv,
            Term::Ptr rhv) {
        return Predicate::Ptr(
                new StorePredicate(lhv, rhv));
    }

    Predicate::Ptr getWritePropertyPredicate(
            Term::Ptr propName,
            Term::Ptr lhv,
            Term::Ptr rhv) {
        return Predicate::Ptr(
                new WritePropertyPredicate(propName, lhv, rhv));
    }

    Predicate::Ptr getAllocaPredicate(
             Term::Ptr lhv,
             Term::Ptr numElements) {
        return Predicate::Ptr(
                new AllocaPredicate(lhv, numElements));
    }

    Predicate::Ptr getMallocPredicate(
                 Term::Ptr lhv,
                 Term::Ptr numElements) {
        return Predicate::Ptr(
                new MallocPredicate(lhv, numElements));
    }



    Predicate::Ptr getBooleanPredicate(
            Term::Ptr v,
            Term::Ptr b) {
        return getEqualityPredicate(v, b, PredicateType::PATH);
    }

    Predicate::Ptr getDefaultSwitchCasePredicate(
            Term::Ptr cond,
            std::vector<Term::Ptr> cases) {
        return Predicate::Ptr(
            new DefaultSwitchCasePredicate(
                cond,
                cases,
                PredicateType::PATH)
        );
    }



    Predicate::Ptr getEqualityPredicate(
            Term::Ptr lhv,
            Term::Ptr rhv,
            PredicateType type = PredicateType::STATE) {
        return Predicate::Ptr(
                new EqualityPredicate(lhv, rhv, type));
    }

    Predicate::Ptr getInequalityPredicate(
            Term::Ptr lhv,
            Term::Ptr rhv,
            PredicateType type = PredicateType::STATE) {
        return Predicate::Ptr(
                new InequalityPredicate(lhv, rhv, type));
    }



    Predicate::Ptr getGlobalsPredicate(
            const std::vector<Term::Ptr>& globals) {
        return Predicate::Ptr(
                new GlobalsPredicate(globals));
    }



    static PredicateFactory::Ptr get() {
        static PredicateFactory::Ptr instance(new PredicateFactory());
        return instance;
    }

private:

    PredicateFactory() {};

};

} /* namespace borealis */

#endif /* PREDICATEFACTORY_H_ */
