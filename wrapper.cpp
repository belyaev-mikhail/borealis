/*
 * wrapper.cpp
 *
 * Created on: Nov 1, 2012
 *     Author: belyaev
 */

#include <iostream>
#include <linux/limits.h>
#include <unistd.h>

#define BACKWARD_HAS_UNWIND 1
#define BACKWARD_HAS_BACKTRACE 0
#define BACKWARD_HAS_DW 0
#define BACKWARD_HAS_BFD 0
#define BACKWARD_HAS_BACKTRACE_SYMBOL 1
#include <backward.hpp>
#include <DB.hpp>

#include <z3/z3++.h>
#include "Driver/gestalt.h"

static backward::SignalHandling sh{std::vector<int>{ SIGABRT, SIGSEGV, SIGILL, SIGINT, SIGTRAP }};

void on_terminate(void) {
    try{ throw; }
    catch (const z3::exception& ex) {
        std::cerr << "z3 exception caught: " << ex.msg() << std::endl;
    }
    abort();
}

static bool th = !!std::set_terminate(on_terminate);

std::string getexepath() {
    char result[PATH_MAX];
    readlink("/proc/self/exe", result, PATH_MAX);
    auto found = std::string(result).find_last_of("/");
    return (std::string(result).substr(0, found) + "/");
}

int main(int argc, const char** argv) {
    if (not leveldb_daemon::DB::isDaemonStarted()) {
        std::string exePath = getexepath();
        auto pid = fork();
        if (pid == 0) {
            std::string db_name = "/tmp/leveldb-testbase";
            std::string socket_name = "/tmp/leveldb-test-server-socket.soc";
            std::string runCmd = exePath + "leveldb_daemon " + db_name + " " + socket_name;
            system(runCmd.c_str());
            return 0;
        }
    }
    auto&& db = leveldb_daemon::DB::getInstance();
    db->setSocket("/tmp/leveldb-test-server-socket.soc");

    using namespace borealis::driver;
    gestalt gestalt{ "wrapper" };
    return gestalt.main(argc, argv);
}
