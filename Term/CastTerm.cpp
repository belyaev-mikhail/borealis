/*
 * CastTerm.cpp
 *
 *  Created on: Jul 3, 2013
 *      Author: sam
 */

#include "Term/CastTerm.h"

namespace borealis {

#include "Util/macros.h"

Type::Ptr CastTerm::resultTypeForCast(llvm::CastType opCode, Type::Ptr rhvt) {
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
        auto rhvSign = Signedness::Unknown;
        if (auto integer = llvm::dyn_cast<borealis::type::Integer>(rhvt)) {
            rhvSign = integer->getSignedness();
        }
        return TyF->getInteger(32, rhvSign);
    }
    case CastType::IntToSLong:      return TyF->getInteger(64, Signedness::Signed);
    case CastType::IntToULong:      return TyF->getInteger(64, Signedness::Unsigned);
    case CastType::NoCast:          return rhvt;
    default: BYE_BYE(Type::Ptr, "Unreachable!");
    }
}

llvm::CastType CastTerm::castForTypes(Type::Ptr lhvt, Type::Ptr rhvt) {
    using llvm::dyn_cast;
    using llvm::CastType;
    using llvm::Signedness;

    if (auto lhvi = dyn_cast<type::Integer>(lhvt)) {
        if (auto rhvi = dyn_cast<type::Integer>(rhvt)) {
            if (lhvi->getBitsize() > 32) {
                return rhvi->getSignedness() == Signedness::Signed ?
                                            CastType::IntToSLong :
                                            CastType::IntToULong;
            } else {
                return CastType::LongToInt;
            }
        } else if (llvm::isa<type::Float>(rhvt)) {
            if (lhvi->getBitsize() > 32) {
                return lhvi->getSignedness() == Signedness::Signed ?
                                            CastType::FloatToSLong :
                                            CastType::FloatToULong;
            } else {
                return lhvi->getSignedness() == Signedness::Signed ?
                                            CastType::FloatToSInt :
                                            CastType::FloatToUInt;
            }
        }
    } else if (llvm::isa<type::Float>(lhvt)) {
        if (auto rhvi = dyn_cast<type::Integer>(rhvt)) {
            if (rhvi->getBitsize() > 32) {
                return rhvi->getSignedness() == Signedness::Signed ?
                                            CastType::SLongToFloat :
                                            CastType::ULongToFloat;
            } else {
                return rhvi->getSignedness() == Signedness::Signed ?
                                            CastType::SIntToFloat :
                                            CastType::UIntToFloat;
            }
        }
    }
    BYE_BYE(CastType, "Unreachable!");
}

#include "Util/unmacros.h"

} // namespace borealis
