//
// Created by kivi on 18.12.15.
//

#include "Util/hash.hpp"
#include "FunctionIdentifier.h"

namespace borealis {

bool FunctionIdentifier::equals(FunctionIdentifier* other) {
    return this->name_ == other->name_ &&
           this->rettype_ == other->rettype_;
}

size_t FunctionIdentifier::hashCode() {
    return util::hash::defaultHasher()(name_, rettype_);
}

void FunctionIdentifier::add(FunctionIdentifier* other) {
    this->calls_ += other->calls_;
}

}   /* namespace borealis */