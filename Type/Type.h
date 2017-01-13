/*
 * Type/Type.h
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
 * using the template file Type/base.h.hst
 * 
 * DO NOT EDIT THIS FILE DIRECTLY
 */

#ifndef TYPE_H
#define TYPE_H

#include <memory>
#include <string>

#include "Protobuf/Gen/Type/Type.pb.h"
#include "Util/typeindex.hpp"

namespace borealis {

namespace proto { class Type; }

/** protobuf -> Type/Type.proto
package borealis.proto;

message Type {
    extensions 1 to 15;
}

**/

class Type : public ClassTag, public std::enable_shared_from_this<const Type> {
protected:
    Type(const Type&) = default;
    
public:
    Type(id_t id) : ClassTag(id) {};

    typedef std::shared_ptr<const Type> Ptr;
    typedef std::unique_ptr<proto::Type> ProtoPtr;

};

} // namespace borealis

namespace std {
template<>
struct hash<borealis::Type::Ptr> {
    size_t operator()(const borealis::Type::Ptr& t) const {
        return reinterpret_cast<size_t>(t.get());
    }
};
template<>
struct hash<const borealis::Type::Ptr> {
    size_t operator()(const borealis::Type::Ptr& t) const {
        return reinterpret_cast<size_t>(t.get());
    }
};
} // namespace std

#endif //TYPE_H"