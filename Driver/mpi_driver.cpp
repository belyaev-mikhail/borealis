//
// Created by abdullin on 10/10/16.
//

#include "mpi_driver.h"
#include "Util/util.h"

#include "Util/macros.h"

namespace borealis {
namespace mpi {

MPI_Driver::MPI_Driver() : ObjectLevelLogging("mpi"),
                           globalRank_(MPI::COMM_WORLD.Get_rank()),
                           size_(MPI::COMM_WORLD.Get_size()),
                           intra_(MPI::COMM_WORLD.Split(MPI_COMM_TYPE_SHARED, 0)),
                           localRank_(intra_.Get_rank()){}

MPI_Driver::MPI_Driver(Rank rank, int size) : ObjectLevelLogging("mpi"),
                                              globalRank_(rank),
                                             size_(size) {}

bool MPI_Driver::isMPI() const {
    return size_ > 1;
}

bool MPI_Driver::isGlobalRoot() const {
    return globalRank_.isRoot();
}

bool MPI_Driver::isLocalRoot() const {
    return localRank_.isRoot();
}

void MPI_Driver::sendInteger(const Rank receiver, const IntegerMessage& msg) const {
    //infos() << globalRank_ << " sending message " << msg << " to " << receiver << endl;
    auto buffer = msg.getData();
    MPI_Send(&buffer, 1, MPI_INT, receiver, msg.getTag(), MPI_COMM_WORLD);
}

const IntegerMessage MPI_Driver::receiveInteger(const Rank source) {
    auto buffer = 0;
    MPI_Recv(&buffer, 1, MPI_INT, source, ANY, MPI_COMM_WORLD, &status_);
    auto msg = IntegerMessage{ buffer, status_.MPI_TAG };
    //infos() << globalRank_ << " receiving message " << msg << " from " << Rank(status_.MPI_SOURCE) << endl;
    return msg;
}

void MPI_Driver::sendBytesArray(const Rank receiver, const BytesArrayMessage& msg) const {
    //infos() << rank_ << " sending message " << msg << " to " << receiver << endl;
    int size = msg.getData().size();
    sendInteger(receiver, {size, status_.MPI_TAG});
    MPI_Send(msg.getData().c_str(), size, MPI_BYTE, receiver, msg.getTag(), MPI_COMM_WORLD);
}

const BytesArrayMessage MPI_Driver::receiveBytesArray(const Rank source) {
    int size = receiveInteger(source).getData();
    char* buffer = new char[size];
    MPI_Recv(buffer, size, MPI_BYTE, getStatus().source_, ANY, MPI_COMM_WORLD, &status_);
    std::string res(buffer);
    delete[] buffer;
    auto msg = BytesArrayMessage{ res.substr(0,size), status_.MPI_TAG };
    //infos() << rank_ << " receiving message " << msg << " from " << Rank(status_.MPI_SOURCE) << endl;
    return msg;
}

void MPI_Driver::terminateAll() const {
    ASSERT(globalRank_.isRoot(), "Trying to terminate all from not-root");
    for (auto receiver = 1; receiver < size_; ++receiver) {
        sendInteger(receiver, { 0, Tag::TERMINATE });
    }
}

void MPI_Driver::terminate(Rank receiver) const {
    ASSERT(not receiver.isRoot(), "Trying to terminate root");
    sendInteger(receiver, { ANY, Tag::TERMINATE });
}

Rank MPI_Driver::getGlobalRank() const {
    return globalRank_;
}

Rank MPI_Driver::getLocalRank() const {
    return localRank_;
}

Rank MPI_Driver::getGlobalRankOfLocalRoot() const {
    return globalRank_-localRank_;
}

int MPI_Driver::getSize() const {
    return size_;
}

int MPI_Driver::getNodeSize() const {
    return intra_.Get_size();
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

std::ostream& operator<<(std::ostream& s, const IntegerMessage& msg) {
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

borealis::logging::logstream& operator<<(borealis::logging::logstream& s, const IntegerMessage& msg) {
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