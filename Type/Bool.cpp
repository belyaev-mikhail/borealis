/*
 * Type/Bool.cpp
 * This file is generated from the following haskell datatype representation:
 * 
 * data Type = 
    Integer { bitsize :: UInt, signedness :: LLVMSignedness } |
    Bool |
    Float |
    UnknownType |
    Pointer { pointed :: Type } |
    Array { element :: Type, size :: Maybe Size } |
    Record { name :: String, body :: RecordBodyRef } |
    TypeError { message :: String } |
    Function { retty :: Type, args :: [Type] }
      deriving (Show, Eq, Data, Typeable)
 * 
 * stored in Type/Type.datatype
 * using the template file Type/derived.cpp.hst
 * 
 * DO NOT EDIT THIS FILE DIRECTLY
 */

#include "Type/Bool.h"