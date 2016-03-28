/*
 * wrapper.cpp
 *
 * Created on: Nov 1, 2012
 *     Author: belyaev
 */

#include <iostream>
#include <unistd.h>

#define BACKWARD_HAS_UNWIND 1
#define BACKWARD_HAS_BACKTRACE 0
#define BACKWARD_HAS_DW 0
#define BACKWARD_HAS_BFD 0
#define BACKWARD_HAS_BACKTRACE_SYMBOL 1
#include <backward.hpp>

#include <z3/z3++.h>
#include "leveldb-mp/include/DB.hpp"
#include "Driver/gestalt.h"

static backward::SignalHandling sh{std::vector<int>{ SIGABRT, SIGSEGV, SIGILL, SIGINT }};

void on_terminate(void) {
    try{ throw; }
    catch (const z3::exception& ex) {
        std::cerr << "z3 exception caught: " << ex.msg() << std::endl;
    }
    abort();
}

static bool th = !!std::set_terminate(on_terminate);

int main(int argc, const char** argv) {
    if (not leveldb_daemon::DB::isDaemonStarted()) {
        auto pid = fork();
        if (pid == 0) {
            system("./lib/leveldb-mp/leveldb_daemon /tmp/leveldb-testbase /tmp/leveldb-test-server-socket.soc");
            return 0;
        }
    }
    auto&& db = leveldb_daemon::DB::getInstance();
    db->setSocket("/tmp/leveldb-test-server-socket.soc");

    using namespace borealis::driver;
    gestalt gestalt{ "wrapper" };
    return gestalt.main(argc, argv);
}
