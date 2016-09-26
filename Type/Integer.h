/*
 * Type/Integer.h
 * This file is generated from the following haskell datatype representation:
 * 
 * data Type = 
    Integer { bitsize :: UInt, signedness :: LLVMSignedness } |
    Bool |
    Float |
    UnknownType |
    Pointer { pointed :: Type, memspace :: Size } |
    Array { element :: Type, size :: Maybe Size } |
    Record { name :: String, body :: RecordBodyRef } |
    TypeError { message :: String } |
    Function { retty :: Type, args :: [Type] }
      deriving (Show, Eq, Data, Typeable)

 * 
 * stored in Type/Type.datatype
 * using the template file Type/derived.h.hst
 * 
 * DO NOT EDIT THIS FILE DIRECTLY
 */

#ifndef INTEGER_H
#define INTEGER_H

#include "Type/Type.h"
#include "Type/RecordBody.h" // including this is generally fucked up

#include "Util/util.h"

namespace borealis {

class TypeFactory;

namespace type {

/** protobuf -> Type/Integer.proto
import "Type/Type.proto";
import "Type/RecordBodyRef.proto";

import "Util/Signedness.proto";

package borealis.type.proto;

message Integer {
    extend borealis.proto.Type {
        optional Integer ext = 1;
    }

    optional uint32 bitsize = 1;
    optional borealis.proto.Signedness signedness = 2;
}

**/
class Integer : public Type {

    typedef Integer Self;
    typedef Type Base;

    Integer(unsigned int bitsize, llvm::Signedness signedness): Type(class_tag(*this)), bitsize(bitsize), signedness(signedness) {}

public:
    friend class ::borealis::TypeFactory;
    
    static bool classof(const Self*) { return true; }
    static bool classof(const Base* b) { return b->getClassTag() == class_tag<Self>(); }

private:
    unsigned int bitsize;
    llvm::Signedness signedness;

public:
    unsigned int getBitsize() const { return this->bitsize; }
    llvm::Signedness getSignedness() const { return this->signedness; }

};

} // namespace type
} // namespace borealis

#endif // INTEGER_H
