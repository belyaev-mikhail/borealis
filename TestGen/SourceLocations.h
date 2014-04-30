/*
 * SourceLocations.h
 *
 *  Created on: Apr 18, 2014
 *      Author: gagarski
 */

#include <set>
#include <unordered_map>


#include <clang/Basic/SourceLocation.h>

#ifndef SOURCELOCATIONS_H_
#define SOURCELOCATIONS_H_

namespace borealis {

struct FuncLoc {
    const std::string name;
    const unsigned int typeOffset;
    const unsigned int declOffset;
    const unsigned int declEndOffset;
    const unsigned int bodyOffset;
    const unsigned int bodyEndOffset;
};

struct CompareFuncLoc {
    bool operator() (const FuncLoc& first, const FuncLoc& second) {
        return first.declOffset < second.declOffset;
    }
};

typedef std::set<FuncLoc, CompareFuncLoc> FunctionsLocationsInFile;
typedef std::unordered_map<std::string, FunctionsLocationsInFile> FunctionsLocations;

struct IncludeLoc {
    const std::string name;
    const unsigned int hashOffset;
    const bool isAngled;
};

struct CompareIncludeLoc {
    bool operator() (const IncludeLoc& first, const IncludeLoc& second) {
        return first.hashOffset < second.hashOffset;
    }
};

typedef std::set<IncludeLoc, CompareIncludeLoc> IncludesLocationsInFile;
typedef std::unordered_map<std::string, IncludesLocationsInFile> IncludesLocations;

class LocationAnalyseResult {
public:
    typedef std::shared_ptr<LocationAnalyseResult> Ptr;
    typedef IncludesLocationsInFile::iterator includes_iterator;
    typedef FunctionsLocationsInFile::iterator functions_iterator;
    typedef IncludesLocationsInFile::reverse_iterator includes_reverse_iterator;
        typedef FunctionsLocationsInFile::reverse_iterator functions_reverse_iterator;

    LocationAnalyseResult(IncludesLocationsInFile& incLoc,
            FunctionsLocationsInFile& funcLoc) :
                incLoc(incLoc), funcLoc(funcLoc) {};
    includes_iterator includes_begin() { return incLoc.begin(); };
    includes_iterator includes_end() { return incLoc.end(); };
    functions_iterator functions_begin() { return funcLoc.begin(); };
    functions_iterator functions_end() { return funcLoc.end(); };

    includes_reverse_iterator includes_rbegin() { return incLoc.rbegin(); };
    includes_reverse_iterator includes_rend() { return incLoc.rend(); };
    functions_reverse_iterator functions_rbegin() { return funcLoc.rbegin(); };
    functions_reverse_iterator functions_rend() { return funcLoc.rend(); };

    bool functions_empty() { return funcLoc.empty(); };
    bool includes_empty() { return incLoc.empty(); };

private:
    IncludesLocationsInFile incLoc;
    FunctionsLocationsInFile funcLoc;
};


} // namespace borealis

#endif /* SOURCELOCATIONS_H_ */
