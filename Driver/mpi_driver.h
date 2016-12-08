//
// Created by abdullin on 9/23/16.
//

#ifndef BOREALIS_MPI_DRIVER_H
#define BOREALIS_MPI_DRIVER_H

#include <mpi.h>
#include <string>

#include "Logging/logger.hpp"
#include "Util/generate_macros.h"


namespace borealis {
namespace mpi {

class Tag {
public:

    enum DataTag { FUNCTION = 0, READY = 1, BYTEARRAY = 2, TERMINATE };

    Tag() : tag_(TERMINATE) {}
    Tag(const DataTag tag) : tag_(tag) {}
    Tag(const int tag) {
        switch (tag) {
            case 0: tag_ = FUNCTION; break;
            case 1: tag_ = READY; break;
            case 2: tag_ = BYTEARRAY; break;
            default: tag_ = TERMINATE;
        }
    }

    operator int() const { return tag_; }
    DataTag get() const { return tag_; }

private:

    DataTag tag_;
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

class IntegerMessage {
public:

    IntegerMessage(const int data, const Tag& tag): data_(data), tag_(tag) {}

    friend std::ostream& operator<<(std::ostream& s, const IntegerMessage& msg);
    friend borealis::logging::logstream& operator<<(borealis::logging::logstream& s, const IntegerMessage& msg);

    int getData() const { return data_; }
    const Tag& getTag() const { return tag_; }

    GENERATE_EQ(IntegerMessage, data_, tag_);

private:

    int data_;
    Tag tag_;
};

class BytesArrayMessage {

public:

    BytesArrayMessage(const std::string data, const Tag& tag): data_(data), tag_(tag) {}

    friend std::ostream& operator<<(std::ostream& s, const BytesArrayMessage& msg);
    friend borealis::logging::logstream& operator<<(borealis::logging::logstream& s, const BytesArrayMessage& msg);

    const std::string& getData() const { return data_; }

    const Tag& getTag() const { return tag_; }

    GENERATE_EQ(BytesArrayMessage, data_, tag_);

private:

    std::string data_;
    Tag tag_;
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

struct Communicator {
    MPI_Comm communicator_;

    Communicator();
    Communicator(const MPI_Comm& comm) : communicator_(comm) {}
};

class MPI_Driver : public logging::ObjectLevelLogging<MPI_Driver> {
public:

    static constexpr int ANY = -1;

    MPI_Driver();

    bool isMPI() const { return globalSize_ > 1; }
    bool isRoot() const { return globalRank_.isRoot(); }
    bool isLocalRoot() const { return localRank_.isRoot(); }

    const Rank& getRank() const { return globalRank_; }
    const Rank& getLocalRank() const { return localRank_; }
    Rank getLocalRoot() const { return globalRank_ - localRank_; }
    int getSize() const { return globalSize_; }
    int getNodeSize() const { return localSize_; }
    Status getStatus() const { return status_; }

    void sendInteger(const Rank receiver, const IntegerMessage& msg) const;
    void broadcastInteger(const Communicator comm, const IntegerMessage& msg) const;
    IntegerMessage receiveInteger(const Rank source = ANY);

    void sendBytesArray(const Rank receiver, const BytesArrayMessage& msg) const;
    void broadcastBytesArray(const Communicator comm, const BytesArrayMessage& msg) const;
    BytesArrayMessage receiveBytesArray(const Rank source = ANY);

    Communicator getRootsCommunicator() const;

    void terminate(const Rank receiver) const;
    // should be root to call that
    void terminateAll() const;

private:

    Rank globalRank_;
    Rank localRank_;
    int globalSize_;
    int localSize_;
    MPI_Status status_;
};

}   /* namespace mpi */
}   /* namespace borealis */

#include "Util/generate_unmacros.h"

#endif //BOREALIS_MPI_DRIVER_H
