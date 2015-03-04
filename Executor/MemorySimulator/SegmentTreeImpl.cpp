/*
 * SegmentTreeImpl.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: belyaev
 */

#include <memory>

#include <Executor/MemorySimulator/SegmentTreeImpl.h>

#include "Logging/tracer.hpp"

#include "Util/macros.h"

namespace borealis {

namespace {

struct allocator: stateInvalidatingTraverser {
    const SegmentNode::MemoryState state;
    const SegmentNode::MemoryStatus status;
    const SimulatedPtrSize size;

    allocator(
        SegmentNode::MemoryState state,
        SegmentNode::MemoryStatus status,
        SimulatedPtrSize size
    ): state(state), status(status), size(size) {
        TRACE_FUNC;
        TRACE_PARAM(state);
        TRACE_PARAM(status);
        TRACE_PARAM(size);
    };

    bool handleChunk(
            SegmentTree* tree,
            SimulatedPtrSize minbound,
            SimulatedPtrSize maxbound,
            SegmentNode::Ptr& t,
            SimulatedPtrSize /* where */
        ) {
        TRACE_FUNC;
        ASSERTC(size <= (maxbound - minbound));
        ASSERTC(size <= tree->chunk_size);

        t->reallyAllocated = size;
        t->state = state;
        if(state == SegmentNode::MemoryState::Memset) t->memSetTo = 0;
        t->status = status;

        TRACE_FMT("Allocated chunk segment { %s, %s }", minbound, minbound + size);
        return true;
    }

    bool handlePath(
            SegmentTree* /* tree */,
            SimulatedPtrSize minbound,
            SimulatedPtrSize maxbound,
            SegmentNode::Ptr& t,
            SimulatedPtrSize where) {
        TRACE_FUNC;
        auto available = maxbound - minbound;
        ASSERTC(size <= available);

        TRACE_PARAM(where);
        TRACE_PARAM(t.get());
        TRACE_PARAM(minbound);
        TRACE_PARAM(maxbound);
        TRACE_PARAM(size);

        if(minbound == where
            && (available/2ULL < size && available >= size)) {
            t->reallyAllocated = size;
            t->state = state;
            if(state == SegmentNode::MemoryState::Memset) t->memSetTo = 0;
            t->status = status;

            TRACE_FMT("Allocated segment { %s, %s }", minbound, minbound + size);
            TRACE_PARAM(t->status);
            return true;
        }

        return false;
    }
};

struct storeTraverser: stateInvalidatingTraverser {
    const uint8_t* const data;
    const SimulatedPtrSize size;

    bool didAlloc = false;

    storeTraverser(const uint8_t* data, SimulatedPtrSize size, bool didAlloc = false)
        :data(data), size(size), didAlloc(didAlloc) {
        TRACE_FUNC;
        TRACE_PARAM(data);
        TRACE_PARAM(size);
        TRACE_PARAM(didAlloc);
    }

    void checkAllocation(SegmentNode::Ptr& t, SimulatedPtr minbound, SimulatedPtr where) {
        TRACE_FUNC;
        if(t->status != SegmentNode::MemoryStatus::Unallocated) {
            if(didAlloc) {
                signalInconsistency("Allocated segment inside other allocated segment detected");
            }
            didAlloc = true;
            TRACES() << "Found allocated segment:" << endl;
            TRACE_FMT("Allocated segment { %s, %s }", minbound, minbound + t->reallyAllocated);

            if(where + size >= minbound + t->reallyAllocated) {
                signalIllegalStore(where);
            }
        }
    }

    void handleChunk(SegmentTree* tree,
                    SimulatedPtrSize minbound,
                    SimulatedPtrSize /* maxbound */,
                    SegmentNode::Ptr& t,
                    SimulatedPtrSize where) {
        TRACE_FUNC;

        checkAllocation(t, minbound, where);

        ASSERTC(size <= tree->chunk_size);

        TRACES() << "Storing!" << endl;

        t->state = SegmentNode::MemoryState::Unknown;

        if(!didAlloc) signalIllegalStore(where);
        const auto offset = where - minbound;
        t->allocateChunk(tree->chunk_size);
        std::memcpy(t->chunk.get() + offset, data, size);
    }

    bool handlePath(SegmentTree* tree,
        SimulatedPtrSize minbound,
        SimulatedPtrSize maxbound,
        SegmentNode::Ptr& t,
        SimulatedPtrSize where) {
        TRACE_FUNC;

        auto available = maxbound - minbound;
        auto mid = middle(minbound, maxbound);

        TRACE_PARAM(where);
        TRACE_PARAM(size);
        TRACE_PARAM(available);
        TRACE_PARAM(minbound);
        TRACE_PARAM(maxbound);
        TRACE_PARAM(didAlloc);
        TRACE_PARAM(t->status);
        TRACE_PARAM(t->state);

        checkAllocation(t, minbound, where);

        if(where < mid && where + size > mid) {
            auto leftChunkSize = mid - where;
            auto rightChunkSize = size - leftChunkSize;

            forceChildrenAndDeriveState(*t);

            storeTraverser left{ data, leftChunkSize, didAlloc };
            tree->traverse(where, left, t->left, minbound, mid);
            storeTraverser right{ data + leftChunkSize, rightChunkSize, didAlloc };
            tree->traverse(mid, right, t->right, mid, maxbound);
            return true;
        }
        return false;
    }
};

struct loader: public emptyTraverser {
    uint8_t* ptr = nullptr;
    SegmentNode::MemoryState state = SegmentNode::MemoryState::Unknown;
    uint8_t filledWith = 0xFF;

    void handleEmptyNode(SimulatedPtrSize where) {
        TRACE_FUNC;
        signalIllegalLoad(where);
    }

    bool handleMemState(SegmentNode::Ptr& t) {
        TRACE_FUNC;
        if(t->state != SegmentNode::MemoryState::Unknown) {
            state = t->state;
            filledWith = t->memSetTo;
            return true;
        } else return false;
    }

    void handleChunk(SegmentTree* /* tree */,
        SimulatedPtrSize minbound,
        SimulatedPtrSize /* maxbound */,
        SegmentNode::Ptr& t,
        SimulatedPtrSize where) {
        TRACE_FUNC;
        if(!handleMemState(t)) {
            auto offset = where - minbound;
            ptr = t->chunk.get() + offset;
        }
    }

    bool handlePath(SegmentTree* /* tree */,
        SimulatedPtrSize minbound,
        SimulatedPtrSize maxbound,
        SegmentNode::Ptr& t,
        SimulatedPtrSize where) {
        TRACE_FUNC;

        TRACE_PARAM(where);
        auto available = maxbound - minbound;

        TRACE_PARAM(available);
        TRACE_PARAM(minbound);
        TRACE_PARAM(maxbound);
        TRACE_PARAM(t->status);
        TRACE_PARAM(t->state);

        return handleMemState(t);
    }
};

} /* empty namespace */

void SegmentTree::allocate(
        SimulatedPtr where,
        SimulatedPtrSize size,
        SegmentNode::MemoryState state,
        SegmentNode::MemoryStatus status) {
    TRACE_FUNC;
    TRACE_PARAM(where);
    TRACE_PARAM(size);
    TRACE_PARAM(state);
    TRACE_PARAM(status);

    if(size > (end - start)) signalOutOfMemory(size);

    traverse(where, allocator{state, status, size});
}

void SegmentTree::store(
        SimulatedPtr where,
        const uint8_t* data,
        SimulatedPtrSize size) {
    TRACE_FUNC;
    TRACE_PARAM(where);
    TRACE_PARAM(data);
    TRACE_PARAM(size);

    return traverse(where, storeTraverser{ data, size, false });
}

SegmentTree::intervalState SegmentTree::get(SimulatedPtr where) {
    TRACE_FUNC;

    loader loadTraverser;
    traverse(where, loadTraverser);

    return { loadTraverser.ptr, loadTraverser.state, loadTraverser.filledWith };
}

} /* namespace borealis */

#include "Util/unmacros.h"
