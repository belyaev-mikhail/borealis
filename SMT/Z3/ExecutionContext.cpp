/*
 * ExecutionContext.cpp
 *
 *  Created on: Nov 22, 2012
 *      Author: belyaev
 */

#include "SMT/Z3/ExecutionContext.h"

namespace borealis {
namespace z3_ {

ExecutionContext::ExecutionContext(
    ExprFactory& factory,
    unsigned long long localMemoryStart,
    unsigned long long localMemoryEnd) :
    factory(factory),
    globalPtr(1ULL),
    localPtr(localMemoryStart),
    localMemoryStart(localMemoryStart),
    localMemoryEnd(localMemoryEnd) {
    initGepBounds();
};

////////////////////////////////////////////////////////////////////////////////

ExecutionContext::MemArray ExecutionContext::memory() const {
    return get(MEMORY_ID);
}

void ExecutionContext::memory(const MemArray& value) {
    set(MEMORY_ID, value);
}

void ExecutionContext::initGepBounds() {
    memArrays.emplace(
        GEP_BOUNDS_ID,
        factory.getDefaultMemoryArray(GEP_BOUNDS_ID, 0)
    );
    gepBounds( gepBounds().store(
        factory.getNullPtr(),
        factory.getIntConst(-1)
    ) );
}

ExecutionContext::MemArray ExecutionContext::gepBounds() const {
    return get(GEP_BOUNDS_ID);
}

void ExecutionContext::gepBounds(const MemArray& value) {
    set(GEP_BOUNDS_ID, value);
}

ExecutionContext::MemArray ExecutionContext::get(const std::string& id) const {
    if (not util::containsKey(memArrays, id)) {
        memArrays.emplace(id, factory.getNoMemoryArray(id));
    }
    return memArrays.at(id);
}

void ExecutionContext::set(const std::string& id, const MemArray& value) {
    if (util::containsKey(memArrays, id)) {
        memArrays.erase(id);
    }
    memArrays.emplace(id, value);
}

////////////////////////////////////////////////////////////////////////////////

ExecutionContext::MemArrayIds ExecutionContext::getMemArrayIds() const {
    return util::viewContainerKeys(memArrays).toHashSet();
}

ExecutionContext::MemArray ExecutionContext::getCurrentMemoryContents() {
    return memory();
}

ExecutionContext::MemArray ExecutionContext::getCurrentGepBounds() {
    return gepBounds();
}

////////////////////////////////////////////////////////////////////////////////

ExecutionContext::Pointer ExecutionContext::getGlobalPtr(size_t offsetSize) {
    return getGlobalPtr(offsetSize, factory.getIntConst(offsetSize));
}

ExecutionContext::Pointer ExecutionContext::getGlobalPtr(size_t offsetSize, Integer origSize) {
    auto&& ret = factory.getPtrConst(globalPtr);
    globalPtr += offsetSize;
    gepBounds( gepBounds().store(ret, origSize) );
    return ret;
}

ExecutionContext::Pointer ExecutionContext::getLocalPtr(size_t offsetSize) {
    return getLocalPtr(offsetSize, factory.getIntConst(offsetSize));
}

ExecutionContext::Pointer ExecutionContext::getLocalPtr(size_t offsetSize, Integer origSize) {
    auto&& ret = factory.getPtrConst(localPtr);
    localPtr += offsetSize;
    gepBounds( gepBounds().store(ret, origSize) );
    return ret;
}

////////////////////////////////////////////////////////////////////////////////

ExecutionContext& ExecutionContext::switchOn(
    const std::string& name,
    const std::vector<Choice>& contexts) {
    auto&& merged = ExecutionContext::mergeMemory(name, *this, contexts);

    this->memArrays = merged.memArrays;
    this->globalPtr = merged.globalPtr;
    this->localPtr = merged.localPtr;

    return *this;
}

ExecutionContext ExecutionContext::mergeMemory(
    const std::string& name,
    ExecutionContext defaultContext,
    const std::vector<Choice>& contexts) {
    ExecutionContext res{ defaultContext.factory, 0ULL, 0ULL };

    // Merge pointers
    for (auto&& e : contexts) {
        res.globalPtr = std::max(res.globalPtr, e.second.globalPtr);
        res.localPtr = std::max(res.localPtr, e.second.localPtr);
    }

    // Collect all active memory array ids
    auto&& memArrayIds = util::viewContainer(contexts)
        .fold(
            defaultContext.getMemArrayIds(),
            [](auto&& a, auto&& e) {
                auto ids = e.second.getMemArrayIds();
                a.insert(ids.begin(), ids.end());
                return a;
            }
        );

    // Merge memory arrays
    for (auto&& id : memArrayIds) {
        auto&& alternatives = util::viewContainer(contexts)
            .map([&](auto&& p) { return std::make_pair(p.first, p.second.get(id)); })
            .toVector();

        res.set(id, MemArray::merge(name, defaultContext.get(id), alternatives));
    }

    return res;
};

////////////////////////////////////////////////////////////////////////////////

ExecutionContext::Integer ExecutionContext::getBound(const Pointer& p) {

    auto&& base = factory.getPtrVar("$$__base__$$(" + p.getName() + ")");
    auto&& zero = factory.getIntConst(0);

    auto&& baseSize = readProperty<Integer>(GEP_BOUNDS_ID, base);
    auto&& pSize =  readProperty<Integer>(GEP_BOUNDS_ID, p);

    std::function<Bool(Pointer)> axBody =
        [=](Pointer any) -> Bool {
            return factory.implies(
                UComparable(any).ugt(base) && UComparable(any).ule(p),
                readProperty<Integer>(GEP_BOUNDS_ID, any) == zero
            );
        };
    auto&& ax = UComparable(base).ule(p) &&
                factory.if_(pSize != zero)
                       .then_(base == p)
                       .else_(baseSize != zero && factory.forAll(axBody));
    base = base.withAxiom(ax);

    return factory.if_(UComparable(baseSize).ugt(p - base))
                  .then_(baseSize - (p - base))
                  .else_(zero);
}

void ExecutionContext::writeBound(const Pointer& p, const Integer& bound) {
    writeProperty(GEP_BOUNDS_ID, p, bound);
}

////////////////////////////////////////////////////////////////////////////////

Z3::Bool ExecutionContext::toSMT() const {
    return factory.getTrue();
}

std::ostream& operator<<(std::ostream& s, const ExecutionContext& ctx) {
    using std::endl;
    return s << "ctx state:" << endl
             << "< global offset = " << ctx.globalPtr << " >" << endl
             << "< local offset = " << ctx.localPtr << " >" << endl
             << ctx.memArrays;
}

const std::string ExecutionContext::MEMORY_ID = "$$__memory__$$";
const std::string ExecutionContext::GEP_BOUNDS_ID = "$$__gep_bound__$$";

} // namespace z3_
} // namespace borealis
