/*
 * Type/Pointer.h
 * This file is generated from the following haskell datatype representation:
 * 
 * data Type = Integer { signedness :: LLVMSignedness } | Bool | Float | UnknownType | Pointer { pointed :: Type } | TypeError { message :: String } deriving (Show, Eq, Data, Typeable)
 * 
 * stored in Type/Type.datatype
 * using the template file Type/derived.h.hst
 * 
 * DO NOT EDIT THIS FILE DIRECTLY
 */

#ifndef POINTER_H
#define POINTER_H

#include "Type/Type.h"


namespace borealis {

class TypeFactory;

namespace type {

/** protobuf -> Type/Pointer.proto
import "Type/Type.proto";

package borealis.type.proto;

message Pointer {
    extend borealis.proto.Type {
        optional Pointer ext = 5;
    }
    optional borealis.proto.Type pointed = 1;
}

**/
class Pointer : public Type {

    typedef Pointer Self;
    typedef Type Base;

    Pointer(Type::Ptr pointed): Type(class_tag(*this)), pointed(pointed) {}

public:
    friend class ::borealis::TypeFactory;
    
    static bool classof(const Self*) { return true; }
    static bool classof(const Base* b) { return b->getClassTag() == class_tag<Self>(); }

private:
    Type::Ptr pointed;

public:
    Type::Ptr getPointed() const { return this->pointed; }

};

} // namespace type
} // namespace borealis

#endif // POINTER_H