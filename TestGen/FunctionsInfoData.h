/* 
 * File:   FunctionsInfoData.h
 * Author: maxim
 *
 * Created on 25 Март 2014 г., 16:21
 */

#include <unordered_map>

#ifndef FUNCTIONSINFODATA_H
#define	FUNCTIONSINFODATA_H

struct FunctionsInfoData {

    std::unordered_map<std::string, std::string> locations;
    std::unordered_map<std::string, std::string> modifiedFiles;
};

#endif	/* FUNCTIONSINFODATA_H */

