/*
 * Type/Array.h
 * This file is generated from the following haskell datatype representation:
 * 
 * data Type = 
    Integer { signedness :: LLVMSignedness } |
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
 * using the template file Type/derived.h.hst
 * 
 * DO NOT EDIT THIS FILE DIRECTLY
 */

#ifndef ARRAY_H
#define ARRAY_H

#include "Type/Type.h"
#include "Type/RecordBody.h" // including this is generally fucked up

#include "Util/option.hpp"
#include <cstddef>

namespace borealis {

class TypeFactory;

namespace type {

/** protobuf -> Type/Array.proto
import "Type/Type.proto";
import "Type/RecordBodyRef.proto";



package borealis.type.proto;

message Array {
    extend borealis.proto.Type {
        optional Array ext = 6;
    }

    optional borealis.proto.Type element = 1;
    optional uint32 size = 2;
}

**/
class Array : public Type {

    typedef Array Self;
    typedef Type Base;

    Array(Type::Ptr element, const borealis::util::option<size_t>& size): Type(class_tag(*this)), element(element), size(size) {}

public:
    friend class ::borealis::TypeFactory;
    
    static bool classof(const Self*) { return true; }
    static bool classof(const Base* b) { return b->getClassTag() == class_tag<Self>(); }

private:
    Type::Ptr element;
    borealis::util::option<size_t> size;

public:
    Type::Ptr getElement() const { return this->element; }
    const borealis::util::option<size_t>& getSize() const { return this->size; }

};

} // namespace type
} // namespace borealis

#endif // ARRAY_H
