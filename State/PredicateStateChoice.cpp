/*
 * PredicateStateChoice.cpp
 *
 *  Created on: May 23, 2013
 *      Author: ice-phoenix
 */

#include "State/PredicateStateChoice.h"

#include "Util/macros.h"

namespace borealis {

PredicateStateChoice::PredicateStateChoice(const std::vector<PredicateState::Ptr>& choices) :
        PredicateState(type_id<Self>()),
        choices(choices) {};

PredicateStateChoice::PredicateStateChoice(std::vector<PredicateState::Ptr>&& choices) :
        PredicateState(type_id<Self>()),
        choices(std::move(choices)) {};

PredicateState::Ptr PredicateStateChoice::addPredicate(Predicate::Ptr p) const {
    std::vector<PredicateState::Ptr> newChoices;
    newChoices.reserve(choices.size());

    std::transform(choices.begin(), choices.end(), std::back_inserter(newChoices),
        [&p](PredicateState::Ptr choice) {
            return choice + p;
        }
    );

    return Simplified(new Self(newChoices));
}

logic::Bool PredicateStateChoice::toZ3(Z3ExprFactory& z3ef, ExecutionContext* pctx) const {
    TRACE_FUNC;

    using borealis::logic::Bool;

    auto res = z3ef.getFalse();
    std::vector<std::pair<Bool, ExecutionContext>> memories;
    memories.reserve(choices.size());

    for (auto& choice : choices) {
        ExecutionContext choiceCtx(*pctx);

        auto path = choice->filterByTypes({PredicateType::PATH});

        auto z3state = choice->toZ3(z3ef, &choiceCtx);
        auto z3path = path->toZ3(z3ef, &choiceCtx);

        res = res || z3state;
        memories.push_back(std::make_pair(z3path, choiceCtx));
    }

    pctx->switchOn(memories);

    return res;
}

PredicateState::Ptr PredicateStateChoice::addVisited(const llvm::Value* loc) const {
    std::vector<PredicateState::Ptr> newChoices;
    newChoices.reserve(choices.size());

    std::transform(choices.begin(), choices.end(), std::back_inserter(newChoices),
        [&loc](PredicateState::Ptr choice) {
            return choice << loc;
        }
    );

    return Simplified(new Self(newChoices));
}

bool PredicateStateChoice::hasVisited(std::initializer_list<const llvm::Value*> locs) const {
    // FIXME: akhin Just fix this piece of crap
    for (const auto* loc : locs) {
        if (
            std::any_of(choices.begin(), choices.end(),
                [&loc](PredicateState::Ptr choice) {
                    return choice->hasVisited({loc});
                }
            )
        ) continue;
        else return false;
    }
    return true;
}

PredicateState::Ptr PredicateStateChoice::map(Mapper m) const {
    std::vector<PredicateState::Ptr> mapped;
    mapped.reserve(choices.size());

    std::transform(choices.begin(), choices.end(), std::back_inserter(mapped),
        [&m](PredicateState::Ptr choice) {
            return choice->map(m);
        }
    );

    return Simplified(new Self(mapped));
}

PredicateState::Ptr PredicateStateChoice::filterByTypes(std::initializer_list<PredicateType> types) const {
    std::vector<PredicateState::Ptr> mapped;
    mapped.reserve(choices.size());

    std::transform(choices.begin(), choices.end(), std::back_inserter(mapped),
        [&types](PredicateState::Ptr choice) {
            return choice->filterByTypes(types);
        }
    );

    return Simplified(new Self(mapped));
}

PredicateState::Ptr PredicateStateChoice::filter(Filterer f) const {
    std::vector<PredicateState::Ptr> mapped;
    mapped.reserve(choices.size());

    std::transform(choices.begin(), choices.end(), std::back_inserter(mapped),
        [&f](PredicateState::Ptr choice) {
            return choice->filter(f);
        }
    );

    return Simplified(new Self(mapped));
}

std::pair<PredicateState::Ptr, PredicateState::Ptr> PredicateStateChoice::splitByTypes(std::initializer_list<PredicateType> types) const {
    std::vector<PredicateState::Ptr> yes;
    std::vector<PredicateState::Ptr> no;
    yes.reserve(choices.size());
    no.reserve(choices.size());

    for (auto& choice: choices) {
        auto split = choice->splitByTypes(types);
        yes.push_back(split.first);
        no.push_back(split.second);
    }

    return std::make_pair(
        Simplified(new Self(yes)),
        Simplified(new Self(no))
    );
}

PredicateState::Ptr PredicateStateChoice::sliceOn(PredicateState::Ptr base) const {
    std::vector<PredicateState::Ptr> slices;
    slices.reserve(choices.size());

    std::transform(choices.begin(), choices.end(), std::back_inserter(slices),
        [&base](PredicateState::Ptr choice) {
            return choice->sliceOn(base);
        }
    );

    if (std::all_of(slices.begin(), slices.end(),
        [](PredicateState::Ptr slice) { return slice != nullptr; })) {
        return Simplified(new Self(slices));
    }

    return nullptr;
}

PredicateState::Ptr PredicateStateChoice::simplify() const {
    std::vector<PredicateState::Ptr> simplified;
    simplified.reserve(choices.size());

    std::transform(choices.begin(), choices.end(), std::back_inserter(simplified),
        [](PredicateState::Ptr choice) {
            return choice->simplify();
        }
    );

    std::remove_if(simplified.begin(), simplified.end(),
        [](PredicateState::Ptr choice) { return choice->isEmpty(); });

    if (simplified.size() == 1) {
        return borealis::util::head(simplified);
    } else {
        return PredicateState::Ptr(new Self(simplified));
    }
}

bool PredicateStateChoice::isEmpty() const {
    return std::all_of(choices.begin(), choices.end(),
        [](PredicateState::Ptr choice) {
            return choice->isEmpty();
        }
    );
}

std::string PredicateStateChoice::toString() const {
    return std::accumulate(choices.begin(), choices.end(), std::string("BEGIN_OR\n"),
        [](const std::string& accum, PredicateState::Ptr choice) {
            return accum + "<or>" + choice->toString() + "\n";
        }
    ) + "END_OR";
}

} /* namespace borealis */

#include "Util/unmacros.h"