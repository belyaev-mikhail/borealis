//
// Created by kivi on 18.12.15.
//

#include "Contract.h"

namespace borealis {

void borealis::Contract::add(borealis::Contract* other) {
    for (auto&& state: other->data()) {
        this->data_.push_back(state);
    }
}

}   /* namespace borealis */