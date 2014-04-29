/*
 * wrapper.cpp
 *
 * Created on: Nov 1, 2012
 *     Author: belyaev
 */

#include <cstdlib>
#include <ctime>

#include "Driver/gestalt.h"

int main(int argc, const char** argv) {
    srand(time(0));
    
    using namespace borealis::driver;
    gestalt gestalt{ "wrapper" };
    return gestalt.main(argc, argv);
}
