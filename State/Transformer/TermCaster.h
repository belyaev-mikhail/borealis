/*
 * TermCaster.h
 *
 *  Created on: Aug 1, 2014
 *      Author: sam
 */

#ifndef TERMCASTER_H_
#define TERMCASTER_H_

#include "State/Transformer/Transformer.hpp"

namespace borealis {

class TermCaster : public Transformer<TermCaster> {

    typedef Transformer<TermCaster> Base;

public:

    explicit TermCaster(FactoryNest FN) : Base(FN) {}

    Term::Ptr transformCmpTerm(CmpTermPtr trm) {
         auto whatToWhat = FN.Term->getCastTerm(trm->getLhv(), trm->getRhv());
         return FN.Term->getCmpTerm(
                     trm->getOpcode(),
                     whatToWhat.first,
                     whatToWhat.second
         );
     }

     Term::Ptr transformBinaryTerm(BinaryTermPtr trm) {
         auto whatToWhat = FN.Term->getCastTerm(trm->getLhv(), trm->getRhv());
         return FN.Term->getBinaryTerm(
                     trm->getOpcode(),
                     whatToWhat.first,
                     whatToWhat.second
         );
     }

};

} // namespace borealis



#endif /* TERMCASTER_H_ */
