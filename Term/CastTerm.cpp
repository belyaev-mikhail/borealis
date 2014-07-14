/*
 * CastTerm.cpp
 *
 *  Created on: Jul 3, 2013
 *      Author: sam
 */

#include "Term/CastTerm.h"

namespace borealis {

#include "Util/macros.h"

Type::Ptr CastTerm::resultTypeForCast(llvm::CastType opCode, Type::Ptr from) {
    using llvm::CastType;
    using llvm::Signedness;
    auto TyF = TypeFactory::get();

    switch(opCode) {
    case CastType::SIntToFloat:     case CastType::SLongToFloat :
    case CastType::UIntToFloat:     case CastType::ULongToFloat :
            return TyF->getFloat();
    case CastType::FloatToSInt:     return TyF->getInteger(32, Signedness::Signed);
    case CastType::FloatToSLong:    return TyF->getInteger(64, Signedness::Signed);
    case CastType::FloatToUInt:     return TyF->getInteger(32, Signedness::Unsigned);
    case CastType::FloatToULong:    return TyF->getInteger(64, Signedness::Unsigned);
    case CastType::LongToInt: {
        auto fromSign = Signedness::Unknown;
        if (auto integer = llvm::dyn_cast<borealis::type::Integer>(from)) {
            fromSign = integer->getSignedness();
        }
        return TyF->getInteger(32, fromSign);
    }
    case CastType::IntToSLong:      return TyF->getInteger(64, Signedness::Signed);
    case CastType::IntToULong:      return TyF->getInteger(64, Signedness::Unsigned);
    case CastType::NoCast:          return from;
    default: BYE_BYE(Type::Ptr, "Unreachable!");
    }
}

llvm::CastType CastTerm::castForTypes(Type::Ptr from, Type::Ptr to) {
    using llvm::dyn_cast;
    using llvm::CastType;
    using llvm::Signedness;

    if (auto toi = dyn_cast<type::Integer>(to)) {
        if (auto fromi = dyn_cast<type::Integer>(from)) {
            if (toi->getBitsize() > 32) {
                return fromi->getSignedness() == Signedness::Signed ?
                                            CastType::IntToSLong :
                                            CastType::IntToULong;
            } else {
                return CastType::LongToInt;
            }
        } else if (llvm::isa<type::Float>(from)) {
            if (toi->getBitsize() > 32) {
                return toi->getSignedness() == Signedness::Signed ?
                                            CastType::FloatToSLong :
                                            CastType::FloatToULong;
            } else {
                return toi->getSignedness() == Signedness::Signed ?
                                            CastType::FloatToSInt :
                                            CastType::FloatToUInt;
            }
        }
    } else if (llvm::isa<type::Float>(to)) {
        if (auto fromi = dyn_cast<type::Integer>(from)) {
            if (fromi->getBitsize() > 32) {
                return fromi->getSignedness() == Signedness::Signed ?
                                            CastType::SLongToFloat :
                                            CastType::ULongToFloat;
            } else {
                return fromi->getSignedness() == Signedness::Signed ?
                                            CastType::SIntToFloat :
                                            CastType::UIntToFloat;
            }
        }
    }
    BYE_BYE(CastType, "Unreachable!");
}

#include "Util/unmacros.h"

} // namespace borealis
