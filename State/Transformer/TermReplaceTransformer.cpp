//
// Created by kivi on 22.06.15.
//

#include "TermReplaceTransformer.h"
#include "../../Util/collections.hpp"

namespace borealis {

TermReplaceTransformer::TermReplaceTransformer(const FactoryNest &fn, const TermMap& map)  : Base(fn), termMap(map) {}

Term::Ptr TermReplaceTransformer::transformTerm(Term::Ptr term) {
    if (auto&& optRef = util::at(termMap, term)) {
        return optRef.getUnsafe();
    }
    return term;
}

}  /* namespace borealis */