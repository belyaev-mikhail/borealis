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
                           globalSize_(MPI::COMM_WORLD.Get_size()) {
    static MPI_Comm* shmcomm = nullptr;
    if (not shmcomm) {
        shmcomm = new MPI_Comm;
        MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0,
                            MPI_INFO_NULL, shmcomm);
    }
    int shmrank, shmsize;
    MPI_Comm_rank(*shmcomm, &shmrank);
    MPI_Comm_size(*shmcomm, &shmsize);


    localRank_ = shmrank;
    localSize_ = shmsize;
}

void MPI_Driver::sendInteger(const Rank receiver, const IntegerMessage& msg) const {
    auto buffer = msg.getData();
    MPI_Send(&buffer, 1, MPI_INT, receiver, msg.getTag(), MPI_COMM_WORLD);
}


IntegerMessage MPI_Driver::broadcastInteger(const Communicator comm, const IntegerMessage& msg) const {
    auto buffer = msg.getData();
    MPI_Bcast(&buffer, 1, MPI_INT, mpi::Rank::ROOT, comm.communicator_);
    return IntegerMessage{buffer, msg.getTag()};
}

IntegerMessage MPI_Driver::receiveInteger(const Rank source) {
    auto buffer = 0;
    MPI_Recv(&buffer, 1, MPI_INT, source, ANY, MPI_COMM_WORLD, &status_);
    return IntegerMessage{ buffer, status_.MPI_TAG };
}

void MPI_Driver::sendBytesArray(const Rank receiver, const BytesArrayMessage& msg) const {
    int size = msg.getData().size();
    sendInteger(receiver, {size, msg.getTag()});
    MPI_Send(msg.getData().c_str(), size, MPI_BYTE, receiver, msg.getTag(), MPI_COMM_WORLD);
}


BytesArrayMessage MPI_Driver::broadcastBytesArray(const Communicator comm, const BytesArrayMessage& msg) const {
    int size = msg.getData().size();
    auto&& sizeMsg = broadcastInteger(comm, {size, msg.getTag()});
    char* buffer = new char[sizeMsg.getData()];
    msg.getData().copy(buffer, sizeMsg.getData());
    MPI_Bcast(buffer, sizeMsg.getData(), MPI_BYTE, mpi::Rank::ROOT, comm.communicator_);
    std::string res(buffer);
    delete[] buffer;
    return BytesArrayMessage{ res.substr(0, sizeMsg.getData()), status_.MPI_TAG };
}

BytesArrayMessage MPI_Driver::receiveBytesArray(const Rank source) {
    auto size = receiveInteger(source).getData();
    char* buffer = new char[size];
    MPI_Recv(buffer, size, MPI_BYTE, getStatus().source_, ANY, MPI_COMM_WORLD, &status_);
    std::string res(buffer);
    delete[] buffer;
    return BytesArrayMessage{ res.substr(0, size), status_.MPI_TAG };
}

void MPI_Driver::terminateAll() const {
    ASSERT(globalRank_.isRoot(), "Trying to terminate all from not-root");
    for (auto receiver = 1; receiver < globalSize_; ++receiver) {
        sendInteger(receiver, { 0, Tag::TERMINATE });
    }
}

void MPI_Driver::terminate(Rank receiver) const {
    ASSERT(not receiver.isRoot(), "Trying to terminate root");
    sendInteger(receiver, { ANY, Tag::TERMINATE });
}

Communicator MPI_Driver::getRootsCommunicator() const {
    int color = (localRank_ == 0) ? 0 : MPI_UNDEFINED;
    static MPI_Comm* communicator = nullptr;
    if (not communicator) {
        communicator = new MPI_Comm;
        MPI_Comm_split_type(MPI_COMM_WORLD, color, 0, MPI_INFO_NULL, communicator);
    }
    return Communicator{*communicator};
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