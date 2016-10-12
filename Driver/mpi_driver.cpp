//
// Created by abdullin on 10/10/16.
//

#include "mpi_driver.h"
#include "Util/util.h"

#include "Util/macros.h"

namespace borealis {
namespace mpi {

MPI_Driver::MPI_Driver() : ObjectLevelLogging("mpi"),
                           rank_(MPI::COMM_WORLD.Get_rank()),
                           size_(MPI::COMM_WORLD.Get_size()) {}

MPI_Driver::MPI_Driver(Rank rank, int size) : ObjectLevelLogging("mpi"),
                                             rank_(rank),
                                             size_(size) {}

bool MPI_Driver::isMPI() const {
    return size_ > 1;
}

bool MPI_Driver::isRoot() const {
    return rank_.isRoot();
}

void MPI_Driver::send(const Rank receiver, const Message& msg) const {
    infos() << rank_ << " sending message " << msg << " to " << Rank(status_.MPI_SOURCE) << endl;
    MPI_Send(&msg.data_, 1, MPI_INT, receiver, msg.tag_, MPI_COMM_WORLD);
}

const Message MPI_Driver::receive(const Rank source) {
    MPI_Recv(&buffer_, 1, MPI_INT, source, ANY, MPI_COMM_WORLD, &status_);
    auto msg = Message{ buffer_, status_.MPI_TAG };
    infos() << rank_ << " receiving message " << msg << " from " << Rank(status_.MPI_SOURCE) << endl;
    return msg;
}

void MPI_Driver::terminateAll() const {
    ASSERT(rank_.isRoot(), "Trying to terminate all from not-root");
    for (auto receiver = 1; receiver < size_; ++receiver) {
        send(receiver, { 0, Tag::TERMINATE });
    }
}

void MPI_Driver::terminate(Rank receiver) const {
    ASSERT(not receiver.isRoot(), "Trying to terminate root");
    send(receiver, { ANY, Tag::TERMINATE });
}

Rank MPI_Driver::getRank() const {
    return rank_;
}

int MPI_Driver::getSize() const {
    return size_;
}

Status MPI_Driver::getStatus() const {
    return Status{ status_ };
}

std::ostream& operator<<(std::ostream& s, const Rank& rank) {
    if (rank.isRoot())
        s << "[Root]";
    else
        s << "[Consumer " << rank.get() << "]";
    return s;
}

borealis::logging::logstream& operator<<(borealis::logging::logstream& s, const Rank& rank) {
    if (rank.isRoot())
        s << "[Root]";
    else
        s << "[Consumer " << rank.get() << "]";
    return s;

}

std::ostream& operator<<(std::ostream& s, const Message& msg) {
    switch (msg.tag_) {
        case Tag::FUNCTION:
            s << "[function " << msg.data_<< "]";
            break;
        case Tag::READY:
            s << "[ready]";
            break;
        default:
            s << "[terminate]";
    }
    return s;
}


borealis::logging::logstream& operator<<(borealis::logging::logstream& s, const Message& msg) {
    switch (msg.tag_) {
        case Tag::FUNCTION:
            s << "[function " << msg.data_<< "]";
            break;
        case Tag::READY:
            s << "[ready]";
            break;
        default:
            s << "[terminate]";
    }
    return s;
}

constexpr int Rank::ROOT;
constexpr int MPI_Driver::ANY;

}   /* namespace mpi */
}   /* namespace borealis */

#include "Util/unmacros.h"