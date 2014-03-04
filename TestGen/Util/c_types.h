/* 
 * File:   c_types.h
 * Author: maxim
 *
 * Created on 4 Март 2014 г., 13:41
 */

#ifndef C_TYPES_H
#define	C_TYPES_H

namespace borealis {
namespace util {

std::string getCType(const llvm::DIType * type, bool fullModifiers);

} /* namespace util */
} /* namespace borealis */

#endif	/* C_TYPES_HPP */

