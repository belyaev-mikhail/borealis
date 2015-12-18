//
// Created by kivi on 17.12.15.
//

#ifndef BOREALIS_FUNCTIONIDENTIFIER_H
#define BOREALIS_FUNCTIONIDENTIFIER_H

#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>

#include <string>
#include <memory>

#include "Protobuf/Gen/Passes/Contract/FunctionIdentifier.pb.h"

namespace borealis {

/** protobuf -> Passes/Contract/FunctionIdentifier.proto

package borealis.proto;

message FunctionIdentifier {
    optional string FunctionName = 1;
    optional string RetType = 2;
    optional uint32 Calls = 3;
}

**/

class FunctionIdentifier {

public:

    using Ptr = std::shared_ptr<FunctionIdentifier>;
    using ProtoPtr = std::unique_ptr<borealis::proto::FunctionIdentifier>;

    FunctionIdentifier(const std::string& fname,
                       const std::string& rettype,
                       const unsigned int calls) :
            name_(fname), rettype_(rettype), calls_(calls) {}

    FunctionIdentifier(llvm::Function* F, const unsigned int calls) :
            calls_(calls) {
        name_ = F->getName();
        llvm::raw_string_ostream rso(rettype_);
        F->getType()->print(rso);
    }

    FunctionIdentifier(const FunctionIdentifier&) = default;
    FunctionIdentifier(FunctionIdentifier&&)      = default;
    ~FunctionIdentifier()                         = default;

    const std::string& name() const {
        return name_;
    }

    const std::string& rettype() const {
        return rettype_;
    }

    unsigned int calls() const {
        return calls_;
    }

private:

    std::string name_;
    std::string rettype_;
    unsigned int calls_;

};

}   /* namespace borealis */

#endif //BOREALIS_FUNCTIONIDENTIFIER_H
