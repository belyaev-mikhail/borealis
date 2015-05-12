/*
 * Codegen/CType/CInteger.cpp
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
 * using the template file Codegen/CType/derived.cpp.hst
 * 
 * DO NOT EDIT THIS FILE DIRECTLY
 */

#include "Codegen/CType/CInteger.h"