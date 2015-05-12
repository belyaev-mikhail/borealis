/*
 * Codegen/CType/CAlias.h
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

#ifndef CALIAS_H
#define CALIAS_H

#include "Codegen/CType/CType.h"



namespace borealis {

class CAlias : public CType {

    typedef CAlias Self;
    typedef CType Base;

    CAlias(const std::string& name, const CTypeRef& original, CQualifier qualifier): CType(class_tag(*this), name), original(original), qualifier(qualifier) {}

public:
    friend class ::borealis::TypeFactory;
    
    static bool classof(const Self*) { return true; }
    static bool classof(const Base* b) { return b->getClassTag() == class_tag<Self>(); }

private:
    CTypeRef original;
    CQualifier qualifier;

public:
    const CTypeRef& getOriginal() const { return this->original; }
    CQualifier getQualifier() const { return this->qualifier; }

};

} // namespace borealis

#endif // CALIAS_H