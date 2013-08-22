/*
 * command.hpp
 *
 *  Created on: Apr 2, 2012
 *      Author: belyaev
 */

#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include "production.h"
#include "Util/util.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <string>

namespace borealis {
namespace anno {

struct command {
    std::string name_;
    std::list<prod_t> args_;
};

template<class Char>
std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& ost, const command& com) {
    ost << com.name_;
    if(com.args_.empty()) return ost;

    ost << "(" << *borealis::util::head(com.args_);
    for(const prod_t& args : borealis::util::tail(com.args_)) {
        ost <<  "," << *args;
    }
    ost << ")";
    return ost;
}

} //namespace anno
} //namespace borealis

#endif /* COMMAND_HPP_ */
