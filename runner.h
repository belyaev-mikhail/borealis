/*
 * runner.h
 *
 *  Created on: Apr 12, 2013
 *      Author: ice-phoenix
 */

#ifndef RUNNER_H_
#define RUNNER_H_

#include <algorithm>
#include <list>

namespace borealis {

class Runner {

public:

    Runner(const std::string& program);
    Runner& withArg(const std::string& arg);
    int run();

private:

    std::string program;
    std::vector<std::string> args;

};

} /* namespace borealis */

#endif /* RUNNER_H_ */
