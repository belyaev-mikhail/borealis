//
// Created by ice-phoenix on 5/20/15.
//

#include "Reanimator/Memory.h"
#include "Reanimator/MemoryObject.h"
#include "Reanimator/Reanimator.h"

namespace borealis {

class RaisingTypeVisitor {

    using RetTy = MemoryObject;

public:

    RaisingTypeVisitor(const Reanimator& r) : r(r) { }

    RetTy visit(Type::Ptr theType, unsigned long long addr, util::option<long long> value) {
        if (false) { }
#define HANDLE_TYPE(NAME, CLASS) \
        else if (auto resolved = llvm::dyn_cast<type::CLASS>(theType)) { \
            return visit##NAME(*resolved, addr, value); \
        }
#include "Type/Type.def"
#include "Util/macros.h"
        BYE_BYE(RetTy, "Unknown type in RaisingTypeVisitor");
#include "Util/unmacros.h"
    }

    RetTy visitBool(const type::Bool& t, unsigned long long, util::option<long long> value) {
        return MemoryObject{
            t.shared_from_this(),
            value,
            1
        };
    }

    RetTy visitInteger(const type::Integer& i, unsigned long long, util::option<long long> value) {
        return MemoryObject{
            i.shared_from_this(),
            value,
            1
        };
    }

    RetTy visitFloat(const type::Float& f, unsigned long long, util::option<long long> value) {
        return MemoryObject{
            f.shared_from_this(),
            value,
            1
        };
    }

    RetTy visitPointer(const type::Pointer& p, unsigned long long, util::option<long long> value) {
        auto&& pType = p.getPointed();
        auto&& pSize = TypeUtils::getTypeSizeInElems(pType);

        if (value) {
            auto&& base = value.getUnsafe();

            auto&& baseMemoryObject = MemoryObject{
                p.shared_from_this(),
                base,
                r.getArraySize(base) * pSize
            };

            for (auto&& shift : r.getArrayBounds(base)) {
                auto&& nested = visit(pType, base + shift * pSize, r.getResult().derefValueOf(base + shift * pSize));

                baseMemoryObject.add(shift, nested);
            }

            return baseMemoryObject;
        } else {
            auto&& baseMemoryObject = MemoryObject{
                p.shared_from_this(),
                pSize
            };

            return baseMemoryObject;
        }
    }

    RetTy visitRecord(const type::Record& rec, unsigned long long addr, util::option<long long>) {
        auto&& rSize = TypeUtils::getTypeSizeInElems(rec.shared_from_this());

        auto&& baseMemoryObject = MemoryObject{
            rec.shared_from_this(),
            rSize
        };

        for (auto&& indexedField : util::range(0UL, rec.getBody()->get().getNumFields())
                                   ^ util::viewContainer(rec.getBody()->get())) {
            auto&& offset = TypeUtils::getStructOffsetInElems(rec.shared_from_this(), indexedField.first);
            auto&& nested = visit(indexedField.second.getType(), addr + offset,
                                  r.getResult().derefValueOf(addr + offset));

            baseMemoryObject.add(indexedField.first, nested);
        }

        return baseMemoryObject;
    }

    RetTy visitArray(const type::Array& arr, unsigned long long addr, util::option<long long>) {
        auto&& elemType = arr.getElement();
        auto&& elemSize = TypeUtils::getTypeSizeInElems(elemType);

        if (arr.getSize()) {
            auto&& arraySize = arr.getSize().getUnsafe();

            auto&& baseMemoryObject = MemoryObject{
                arr.shared_from_this(),
                arraySize * elemSize
            };

            for (auto&& i = 0U; i < arraySize; ++i) {
                auto&& nested = visit(elemType, addr + i * elemSize, r.getResult().derefValueOf(addr + i * elemSize));

                baseMemoryObject.add(i, nested);
            }

            return baseMemoryObject;
        } else {
            auto&& baseMemoryObject = MemoryObject{
                arr.shared_from_this(),
                elemSize
            };

            return baseMemoryObject;
        }
    }

    RetTy visitUnknownType(const type::UnknownType& ut, unsigned long long, util::option<long long>) {
        return MemoryObject{
            ut.shared_from_this(),
            1
        };
    }

    RetTy visitTypeError(const type::TypeError& te, unsigned long long, util::option<long long>) {
        return MemoryObject{
            te.shared_from_this(),
            1
        };
    }

    RetTy visitFunction(const type::Function& f, unsigned long long, util::option<long long>) {
        return MemoryObject{
            f.shared_from_this(),
            1
        };
    }

private:

    Reanimator r;

};

ReanimatorView::ReanimatorView(const Reanimator& r, Term::Ptr term) :
    r(r), term(term) { }

borealis::logging::logstream& operator<<(
    borealis::logging::logstream& s,
    const ReanimatorView& rv
) {
    if (auto&& value = rv.r.getResult().valueOf(rv.term->getName())) {
        s << "Raising (" << *rv.term->getType() << ") " << rv.term << " from the dead..." << endl;

        s << RaisingTypeVisitor(rv.r).visit(rv.term->getType(), 0xDEADBEEF, value) << endl;

    } else {
        s << "Cannot raise " << rv.term << " from the dead!" << endl;
    }
    return s;
}

Reanimator::Reanimator(const smt::SatResult& result, const ArrayBoundsCollector::ArrayBounds& arrayBounds_) :
    result(result) {
    processArrayBounds(arrayBounds_);
}

const smt::SatResult& Reanimator::getResult() const {
    return result;
}

const Reanimator::ArrayBoundsMap& Reanimator::getArrayBoundsMap() const {
    return arrayBoundsMap;
}

const Reanimator::ArrayBounds& Reanimator::getArrayBounds(unsigned long long base) const {
    static ArrayBounds const empty{0};
    if (util::contains(arrayBoundsMap, base)) {
        return arrayBoundsMap.at(base);
    } else {
        return empty;
    }
}

long long Reanimator::getArraySize(unsigned long long base) const {
    return 1 + util::max_element(getArrayBounds(base));
}

void Reanimator::processArrayBounds(const ArrayBoundsCollector::ArrayBounds& arrayBounds_) {

    static long long const null_ptr = 0;
    static long long const invalid_ptr = -1;

    for (auto&& e : arrayBounds_) {
        auto&& pType = llvm::dyn_cast<type::Pointer>(e.first->getType());
        auto&& pElemTypeSize = (long long) TypeUtils::getTypeSizeInElems(pType->getPointed());

        for (auto&& base : result.valueOf(e.first->getName())) {
            for (auto&& bound : e.second) {
                for (auto&& bound_ : result.valueOf(bound->getName())) {
                    if (null_ptr == bound_) continue;
                    if (invalid_ptr == bound_) continue;

                    auto&& delta = bound_ - base;
                    auto&& shift = delta / pElemTypeSize;

                    if (delta < 0) shift -= 1;

                    arrayBoundsMap[base].insert(shift);
                }
            }
        }
    }
}

ReanimatorView raise(const Reanimator& r, Term::Ptr value) {
    return ReanimatorView(r, value);
}

} // namespace borealis
