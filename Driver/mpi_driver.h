//
// Created by abdullin on 9/23/16.
//

#ifndef BOREALIS_MPI_DRIVER_H
#define BOREALIS_MPI_DRIVER_H

#include <mpi.h>

#include "Logging/logger.hpp"
#include "Util/util.h"
#include "Util/macros.h"



namespace borealis {
namespace mpi {

static const int ROOT = 0;

enum DataTag {
    FUNCTION = 0,
    READY = 1,
    TERMINATE
};

class MPI_Driver : public logging::ObjectLevelLogging<MPI_Driver> {

    using Data = int;
    using Tag = int;

public:

    static const int ROOT = 0;

    MPI_Driver() : ObjectLevelLogging("mpi") {
        rank_ = MPI::COMM_WORLD.Get_rank();
        size_ = MPI::COMM_WORLD.Get_size();
    }

    MPI_Driver(int rank, int size) : ObjectLevelLogging("mpi"), rank_(rank), size_(size) {}

    bool isRoot() const {
        return rank_ == ROOT;
    }

    bool isMPI() const {
        return size_ > 1;
    }

    void send(int receiver, Data data, Tag tag) const {
        infos() << rank_ << " sending message " << DataTag(tag)
                << " to " << receiver << endl;
        MPI_Send(&data, 1, MPI_INT, receiver, tag, MPI_COMM_WORLD);
    }

    Data receive(int source = MPI_ANY_SOURCE) {
        MPI_Recv(&buffer_, 1, MPI_INT, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status_);
        infos() << rank_ << " receiving message " << DataTag(status_.MPI_TAG)
                << " from " << status_.MPI_SOURCE << endl;
        return buffer_;
    }

    const MPI_Status& getStatus() const {
        return status_;
    }

    void terminateAll() const {
        ASSERTC(isRoot());
        for (auto receiver = 1; receiver < size_; ++receiver) {
            send(receiver, 0, DataTag::TERMINATE);
        }
    }

    void terminate(int receiver) const {
        ASSERTC(receiver != 0);
        send(receiver, 0, DataTag::TERMINATE);
    }

    int getRank() const {
        return rank_;
    }

    int getSize() const {
        return size_;
    }

private:

    int rank_;
    int size_;
    Data buffer_;
    MPI_Status status_;

};

std::ostream& operator<<(std::ostream& s, const DataTag& tag) {
    switch (tag) {
        case FUNCTION:
            s << "[function]";
            break;
        case READY:
            s << "[ready]";
            break;
        default:
            s << "[terminate]";
    }
    return s;
}

borealis::logging::logstream& operator<<(borealis::logging::logstream& s, const DataTag& tag) {
    switch (tag) {
        case FUNCTION:
            s << "[function]";
            break;
        case READY:
            s << "[ready]";
            break;
        default:
            s << "[terminate]";
    }
    return s;
}

}   /* namespace mpi */
}   /* namespace borealis */

#include "Util/unmacros.h"

#endif //BOREALIS_MPI_DRIVER_H
