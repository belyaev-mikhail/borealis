//
// Created by abdullin on 9/23/16.
//

#ifndef BOREALIS_MPI_DRIVER_H
#define BOREALIS_MPI_DRIVER_H

#include <mpi.h>

#include "Logging/logger.hpp"
#include "Util/generate_macros.h"


namespace borealis {
namespace mpi {

struct Tag {
    enum DataTag { FUNCTION = 0, READY = 1, TERMINATE };
    DataTag tag_;

    Tag() : tag_(TERMINATE) {}
    Tag(const DataTag tag) : tag_(tag) {}
    Tag(const int tag) {
        switch (tag) {
            case 0: tag_ = FUNCTION; break;
            case 1: tag_ = READY; break;
            default: tag_ = TERMINATE;
        }
    }

    operator int() const { return tag_; }
};

class Rank {
public:

    static constexpr int ROOT = 0;

    Rank() {}
    Rank(const int rank) : rank_(rank) {}

    int get() const { return rank_; }
    bool isRoot() const {return rank_ == ROOT; }

    operator int() const { return rank_; }
    friend std::ostream& operator<<(std::ostream& s, const Rank& rank);
    friend borealis::logging::logstream& operator<<(borealis::logging::logstream& s, const Rank& rank);

private:

    int rank_;
};

struct Message {
    int data_;
    Tag tag_;

    Message(const int data, const Tag& tag): data_(data), tag_(tag) {}

    friend std::ostream& operator<<(std::ostream& s, const Message& msg);
    friend borealis::logging::logstream& operator<<(borealis::logging::logstream& s, const Message& msg);

    GENERATE_EQ(Message, data_, tag_);
};

struct Status {
    Rank source_;
    Tag tag_;
    int error_;

    Status() {}
    Status(const MPI_Status& st) : source_(st.MPI_SOURCE),
                                   tag_(st.MPI_TAG),
                                   error_(st.MPI_ERROR) {}

    GENERATE_EQ(Status, source_, tag_, error_);
    GENERATE_PRINT(Status, source_, tag_, error_);
};

class MPI_Driver : public logging::ObjectLevelLogging<MPI_Driver> {
public:

    static constexpr int ANY = -1;

    MPI_Driver();
    MPI_Driver(Rank rank, int size);

    bool isMPI() const;
    bool isRoot() const;

    void send(const Rank receiver, const Message& msg) const;
    const Message receive(const Rank source = ANY);

    void terminate(const Rank receiver) const;
    // should be root to call that
    void terminateAll() const;

    Rank getRank() const;
    int getSize() const;
    Status getStatus() const;

private:

    Rank rank_;
    int size_;
    int buffer_;
    MPI_Status status_;
};

}   /* namespace mpi */
}   /* namespace borealis */

#include "Util/generate_unmacros.h"

#endif //BOREALIS_MPI_DRIVER_H
