/*
 * ExecutionContext.h
 *
 *  Created on: Aug 5, 2013
 *      Author: sam
 */

#ifndef MATHSAT_EXECUTIONCONTEXT_H_
#define MATHSAT_EXECUTIONCONTEXT_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "SMT/MathSAT/ExprFactory.h"

namespace borealis {
namespace mathsat_ {

class ExecutionContext {

    USING_SMT_LOGIC(MathSAT);
    typedef MathSAT::ExprFactory ExprFactory;

    ExprFactory& factory;
    mutable std::unordered_map<std::string, MemArray> memArrays;
    unsigned long long globalPtr;
    unsigned long long localPtr;

    static constexpr auto MEMORY_ID = "$$__memory__$$";
    MemArray memory() const {
        return get(MEMORY_ID);
    }
    void memory(const MemArray& value) {
        set(MEMORY_ID, value);
    }

    static constexpr auto GEP_BOUNDS_ID = "$$__gep_bound__$$";
    MemArray gepBounds() const {
        return get(GEP_BOUNDS_ID);
    }
    void gepBounds(const MemArray& value) {
        set(GEP_BOUNDS_ID, value);
    }

    MemArray get(const std::string& id) const {
        using borealis::util::containsKey;
        if (containsKey(memArrays, id)) {
            return memArrays.at(id);
        }
        auto ret = factory.getNoMemoryArray();
        memArrays.emplace(id, ret);
        return ret;
    }
    void set(const std::string& id, const MemArray& value) {
        using borealis::util::containsKey;
        if (containsKey(memArrays, id)) {
            memArrays.erase(id);
        }
        memArrays.emplace(id, value);
    }

    typedef std::unordered_set<std::string> MemArrayIds;
    MemArrayIds getMemArrayIds() const {
        auto it = util::iterate_keys(util::begin_end_pair(memArrays));
        return MemArrayIds(it.first, it.second);
    }

public:

    ExecutionContext(ExprFactory& factory, unsigned long long localMemory);
    ExecutionContext(const ExecutionContext&) = default;

    MemArray getCurrentMemoryContents() {
        return memory();
    }
    MemArray getCurrentGepBounds() {
        return gepBounds();
    }

    inline Pointer getGlobalPtr(size_t offsetSize = 1U) {
        auto ret = factory.getPtrConst(globalPtr);
        globalPtr += offsetSize;
        gepBounds( gepBounds().store(ret, factory.getIntConst(offsetSize)));
        return ret;
    }

    inline Pointer getLocalPtr(size_t offsetSize = 1U) {
        auto ret = factory.getPtrConst(localPtr);
        localPtr += offsetSize;
        gepBounds( gepBounds().store(ret, factory.getIntConst(offsetSize)));
        return ret;
    }

////////////////////////////////////////////////////////////////////////////////

    Dynamic readExprFromMemory(Pointer ix, size_t bitSize) {
        return memory().select(ix, bitSize);
    }
    template<class ExprClass>
    ExprClass readExprFromMemory(Pointer ix) {
        return memory().select<ExprClass>(ix);
    }
    template<class ExprClass>
    void writeExprToMemory(Pointer ix, ExprClass val) {
        memory( memory().store(ix, val) );
    }

    Dynamic readProperty(const std::string& id, Pointer ix, size_t bitSize) {
        return get(id).select(ix, bitSize);
    }
    template<class ExprClass>
    ExprClass readProperty(const std::string& id, Pointer ix) {
        return get(id).select<ExprClass>(ix);
    }
    template<class ExprClass>
    void writeProperty(const std::string& id, Pointer ix, ExprClass val) {
        set( id, get(id).store(ix, val) );
    }

////////////////////////////////////////////////////////////////////////////////

    typedef std::pair<Bool, ExecutionContext> Choice;

    ExecutionContext& switchOn(
            const std::string& name,
            const std::vector<Choice>& contexts) {
        auto merged = ExecutionContext::mergeMemory(name, *this, contexts);

        this->memArrays = merged.memArrays;
        this->globalPtr = merged.globalPtr;
        this->localPtr = merged.localPtr;

        return *this;
    }

    static ExecutionContext mergeMemory(
            const std::string& name,
            ExecutionContext defaultContext,
            const std::vector<Choice>& contexts) {
        ExecutionContext res(defaultContext.factory, 0ULL);

        // Merge pointers
        for (const auto& e : contexts) {
            res.globalPtr = std::max(res.globalPtr, e.second.globalPtr);
            res.localPtr = std::max(res.localPtr, e.second.localPtr);
        }

        // Collect all active memory array ids
        auto memArrayIds = std::accumulate(contexts.begin(), contexts.end(),
                defaultContext.getMemArrayIds(),
                [](MemArrayIds a, Choice e) -> MemArrayIds {
                    auto ids = e.second.getMemArrayIds();
                    a.insert(ids.begin(), ids.end());
                    return a;
                }
        );

        // Merge memory arrays
        for (const auto& id : memArrayIds) {
            std::vector<std::pair<Bool, MemArray>> alternatives;
            alternatives.reserve(contexts.size());
            std::transform(contexts.begin(), contexts.end(), std::back_inserter(alternatives),
                    [&id](const Choice& p) { return std::make_pair(p.first, p.second.get(id)); }
            );

            res.set(id, MemArray::merge(name, defaultContext.get(id), alternatives));
        }

        return res;
    }

////////////////////////////////////////////////////////////////////////////////

    Integer getBound(const Pointer& p) {
        return readProperty<Integer>(GEP_BOUNDS_ID, p);
    }

////////////////////////////////////////////////////////////////////////////////

    Bool toSMT() const;

    friend std::ostream& operator<<(std::ostream& s, const ExecutionContext& ctx);
};

std::ostream& operator<<(std::ostream& s, const ExecutionContext& ctx);

} // namespace mathsat_
} // namespace borealis

#endif /* MATHSAT_EXECUTIONCONTEXT_H_ */
