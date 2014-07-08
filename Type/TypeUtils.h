/*
 * TypeUtils.h
 *
 *  Created on: Nov 25, 2013
 *      Author: ice-phoenix
 */

#ifndef TYPEUTILS_H_
#define TYPEUTILS_H_

#include "Type/Type.def"
#include "Util/typeindex.hpp"
#include "Util/util.h"

#include "Util/macros.h"

namespace borealis {

struct TypeUtils {

    static bool isValid(Type::Ptr type) {
        return type->getClassTag() != class_tag<type::TypeError>();
    }

    static bool isInvalid(Type::Ptr type) {
        return ! isValid(type);
    }

    static bool isUnknown(Type::Ptr type) {
        return type->getClassTag() == class_tag<type::UnknownType>();
    }

    static std::string toString(const Type& type) {
        using llvm::isa;
        using llvm::dyn_cast;

        if(auto* integer = dyn_cast<type::Integer>(&type)) {
            if (integer->getBitsize() > 32) {
                return "Long";
            } else {
                return "Integer";
            }
        }
        if(isa<type::Float>(type)) return "Float";
        if(isa<type::Bool>(type)) return "Bool";
        if(isa<type::UnknownType>(type)) return "Unknown";
        if(auto* Ptr = dyn_cast<type::Pointer>(&type)) return TypeUtils::toString(*Ptr->getPointed()) + "*";
        if(auto* Err = dyn_cast<type::TypeError>(&type)) return "<Type Error>: " + Err->getMessage();
        if(auto* Arr = dyn_cast<type::Array>(&type)) {
            std::string ret = TypeUtils::toString(*Arr->getElement()) + "[";
            for(const auto& size : Arr->getSize()) {
                ret += util::toString(size);
            }
            ret += "]";
            return std::move(ret);
        }
        if(auto* Rec = dyn_cast<type::Record>(&type)) {
            std::string ret = Rec->getName() + "{ ";
            const auto& flds = Rec->getBody()->get();
            if(!flds.empty()) {
                const auto& hd = util::head(flds);
                ret += util::toString(hd.getIndex()) +
                       ": " +
                       util::toString(hd.getIds());
                for(const auto& fld : util::tail(flds)) {
                    ret += ", ";
                    ret += util::toString(fld.getIndex()) +
                           ": " +
                           util::toString(fld.getIds());
                }
            }
            ret += " }";
            return std::move(ret);
        }
        if(auto* Fun = dyn_cast<type::Function>(&type)) {
            std::string ret = TypeUtils::toString(*Fun->getRetty()) + "( ";
            const auto& args = Fun->getArgs();
            if(!args.empty()) {
                ret += TypeUtils::toString(*util::head(args));
                for(const auto& arg : util::tail(args)) {
                    ret += ", ";
                    ret += TypeUtils::toString(*arg);
                }
            }
            ret += " )";
            return std::move(ret);
        }

        BYE_BYE(std::string, "Unknown type");
    }

    static unsigned long long getTypeSizeInElems(Type::Ptr type) {
        return TypeSizer().visit(type);
    }

    static unsigned long long getStructOffsetInElems(Type::Ptr type, unsigned idx) {
        if (auto* structType = llvm::dyn_cast<type::Record>(type)) {
            const auto& recordBody = structType->getBody()->get();
            auto res = 0U;
            for (auto i = 0U; i < idx; ++i) {
                res += getTypeSizeInElems(recordBody.at(i).getType());
            }
            return res;
        }

        BYE_BYE(unsigned long long, "Funk you!");
    }

    static llvm::Signedness isSigned(Type::Ptr type) {
        if (auto integer = llvm::dyn_cast<type::Integer>(type)) {
            return integer->getSignedness();
        } else {
            return llvm::Signedness::Unknown;
        }
    }

    static bool isSame(Type::Ptr lhvt, Type::Ptr rhvt) {
        using llvm::dyn_cast;
        if (lhvt->getClassTag() == rhvt->getClassTag()) {
            if (auto lhvi = dyn_cast<type::Integer>(lhvt)) {
                if (auto rhvi = dyn_cast<type::Integer>(rhvt)) {
                    return lhvi->getBitsize() == rhvi->getBitsize();
                }
            }
            return true;
        }
        return false;
    }

};

} // namespace borealis

#include "Util/unmacros.h"

#endif /* TYPEUTILS_H_ */
