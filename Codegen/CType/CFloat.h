/*
 * Codegen/CType/CFloat.h
 * This file is generated from the following haskell datatype representation:
 * 
 * data CType = 
    CInteger { bitsize :: Size, signedness :: LLVMSignedness } |
    CFloat { bitsize:: Size } |
    CPointer { element :: Param CTypeRef } |
    CAlias { original :: Param CTypeRef, qualifier :: Exact CQualifier } |
    CArray { element :: Param CTypeRef, size :: Maybe Size } |
    CStruct { elements :: [Exact CStructMember] } |
    CFunction { resultType :: Param CTypeRef, argumentTypes :: [Param CTypeRef] }
      deriving (Show, Eq, Data, Typeable)

 * 
 * stored in Codegen/CType/CType.datatype
 * using the template file Codegen/CType/derived.h.hst
 * 
 * DO NOT EDIT THIS FILE DIRECTLY
 */

#ifndef CFLOAT_H
#define CFLOAT_H

#include "Codegen/CType/CType.h"
#include "Codegen/CType/CTypeRef.h"
#include "Codegen/CType/CStructMember.h"

#include <cstddef>

namespace borealis {

class CTypeFactory;

class CFloat : public CType {

    typedef CFloat Self;
    typedef CType Base;

    CFloat(const std::string& name, size_t bitsize): CType(class_tag(*this), name), bitsize(bitsize) {}

public:

    friend class ::borealis::CTypeFactory;

    static bool classof(const Self*) { return true; }
    static bool classof(const Base* b) { return b->getClassTag() == class_tag<Self>(); }

private:
    size_t bitsize;

public:
    size_t getBitsize() const { return this->bitsize; }

};

} // namespace borealis

#endif // CFLOAT_H
