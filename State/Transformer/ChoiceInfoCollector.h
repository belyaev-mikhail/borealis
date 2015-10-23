
#ifndef STATE_TRANSFORMER_CHOICEINFOCOLLECTOR_H_
#define STATE_TRANSFORMER_CHOICEINFOCOLLECTOR_H_

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <llvm/IR/Instructions.h>

#include "Transformer.hpp"

namespace borealis {

class ChoiceInfoCollector : public Transformer<ChoiceInfoCollector> {

    using Base = Transformer<ChoiceInfoCollector>;
    using ChoiceInfo = std::vector<std::vector<Predicate::Ptr>>;

public:

    ChoiceInfoCollector(const FactoryNest& fn);

    ChoiceInfo getChoiceInfo() {return choiceInfo;}

    PredicateState::Ptr transformChoice(PredicateStateChoicePtr pred);

    using Base::transformBase;

private:

    ChoiceInfo choiceInfo;

};



}

#endif