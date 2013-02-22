#ifndef FLOAT_H
#define FLOAT_H

#include "Type/Type.h"

namespace borealis {

class Float : public Type {
    typedef Float self;
    typedef Type base;


    Float() : Type(type_id(*this)) {}

public:
    static bool classof(const self*) { return true; }
    static bool classof(const base* b) { return b->getId() == type_id<self>(); }

    friend class TypeFactory;

    
};

} // namespace borealis

#endif // FLOAT_H


