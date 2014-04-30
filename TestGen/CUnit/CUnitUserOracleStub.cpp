/*
 * CUnitOraclesStubs.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: gagarski
 */


#include <iostream>

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/PathV2.h>

#include "Passes/Tracker/SlotTrackerPass.h"
#include "TestGen/CUnit/CUnitUserOracleStub.h"
#include "TestGen/CUnit/util.hpp"
#include "TestGen/Util/c_types.h"

namespace borealis {
namespace util {

template<class ToInsert, class Unit>
bool updateOracleFile(Unit& unit,
            const std::string& fileName, LocationAnalyseResult& oldLocs) {
    int tmpFD;
    llvm::SmallString<256> tmpPath;
    auto err = llvm::sys::fs::unique_file(llvm::Twine(
            llvm::sys::path::stem(fileName)) + ".%%%%%%%%", tmpFD, tmpPath);
    if (err != llvm::errc::success)
        return false;
    close(tmpFD);
    std::ofstream tmpOutputFile;
    tmpOutputFile.open(tmpPath.str(), std::ios::out);
    std::ifstream inputFile;
    inputFile.open(fileName, std::ios::in);
    inputFile.seekg(0, inputFile.end);
    int inFileSize = inputFile.tellg();
    unsigned int inFileRd = 0;
    inputFile.seekg(0, inputFile.beg);
    bool failed = false;

    // Searching for includes in the top of file (the last include before the first function)
    auto includesInsertItr = oldLocs.includes_rend();
    if (!oldLocs.functions_empty()) {
        for (auto include = oldLocs.includes_rbegin();
                include != oldLocs.includes_rend(); include++) {
            if (include->hashOffset < oldLocs.functions_begin()->declOffset) {
                includesInsertItr = include;
                break;
            }

        }
    }

    // Copying old includes
    if (includesInsertItr != oldLocs.includes_rend() && !failed) {
        int toRead = includesInsertItr->hashOffset;
        failed = !util::copyPartOfFile(inputFile, tmpOutputFile, toRead, inFileRd);
        if (!failed) {
            if (includesInsertItr->isAngled) {
                failed = !util::copyUntilChar(inputFile, tmpOutputFile,
                        '>', inFileRd);
            } else {
                // Copying until second quote
                failed = !util::copyUntilChar(inputFile, tmpOutputFile,
                        '\"', inFileRd);
                failed = !util::copyUntilChar(inputFile, tmpOutputFile,
                        '\"', inFileRd);
            }
        }
        tmpOutputFile << static_cast<char>(inputFile.get()); // EOL
        inFileRd += 1;
    }

    // Adding new includes
    auto includes = util::getIncludesForFunctions(unit.funcs.begin(), unit.funcs.end(),
            unit.prototypes);
    auto filteredIncludes = util::view(includes.begin(), includes.end()).filter(
            [&oldLocs](decltype(*includes.begin()) inc) {
                return std::find_if(oldLocs.includes_begin(),
                                    oldLocs.includes_end(),
                    [&inc](decltype(*oldLocs.includes_begin()) oldInc) {
                        return inc == oldInc.name;
                    }
                ) == oldLocs.includes_end();
            }
    );
    if (!filteredIncludes.empty()) {
        util::writeIncludes(filteredIncludes.begin(), filteredIncludes.end(),
                tmpOutputFile, unit.baseDirectory, unit.moduleName);
        tmpOutputFile << "\n";
    }
    // Copying old functions & others until the end of last function's body
    bool lastIsProto = (oldLocs.functions_rbegin())->bodyEndOffset == 0;
    if (!oldLocs.functions_empty() && !failed) {
        int toRead = lastIsProto ?
                (oldLocs.functions_rbegin())->declEndOffset + 1 - inFileRd :
                (oldLocs.functions_rbegin())->bodyEndOffset + 1 - inFileRd;
        failed = !util::copyPartOfFile(inputFile, tmpOutputFile, toRead, inFileRd);
    }
    if (lastIsProto && !failed)
        failed = !util::copyUntilChar(inputFile, tmpOutputFile, ';', inFileRd);
    int firstNonWhiteSpace;
    if (!failed)
        util::copyWhitespaces(inputFile, tmpOutputFile, inFileRd, firstNonWhiteSpace);

    // Inserting new functions
    for (auto& f: unit.funcs) {
        tmpOutputFile << ToInsert(f, unit.stp, unit.mit);
    }
    if (firstNonWhiteSpace != EOF)
        tmpOutputFile << static_cast<char>(firstNonWhiteSpace);
    // Copying rest of file
    int toRead = inFileSize - inFileRd;
    if (!failed)
        failed = !util::copyPartOfFile(inputFile, tmpOutputFile, toRead, inFileRd);
    inputFile.close();
    tmpOutputFile.close();
    bool existed;
    if (failed) {
        llvm::sys::fs::remove(tmpPath.str(), existed);
    } else {
        llvm::sys::fs::remove(fileName, existed);
        llvm::sys::fs::rename(llvm::Twine(tmpPath), llvm::Twine(fileName));
    }
    if (failed)
        std::cout << "Failed on" << fileName << "\n";
    return !failed;
}

std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubDecl& decl) {
    auto* function = decl.function;
    auto* f = const_cast<llvm::Function*>(function);
    auto* st = decl.stp.getSlotTracker(*f);
    auto fn = FactoryNest(st);
    os << "int " << function->getName() << "Oracle(";
    std::string args;
    if (!function->arg_empty()) {
        for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++) {
            auto arg_ = const_cast<llvm::Argument *>(&(*arg));
            auto argTerm = fn.Term->getArgumentTerm(arg_);
            auto type = decl.mit.locate(arg_).front().type;
            args += getCType(type, CTypeModifiersPolicy::DISCARD);
            args += " ";
            args += arg->getName();
            args += ", ";
        }
    }
    args += getCType(decl.mit.locate(f).front().type, CTypeModifiersPolicy::DISCARD);
    args += " ";
    args += TestDumpPass::getResultNameForFunction(f);
    os << args;
    os << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubDefinition& stub) {
    os << CUnitUserOracleStubDecl(stub.function, stub.stp, stub.mit);
    os << " {\n";
    os << "    // Put your oracle code here.\n";
    os << "    return 1;\n";
    os << "}\n\n";
    return os;
}



std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubProto& proto) {
    os << CUnitUserOracleStubDecl(proto.function, proto.stp, proto.mit);
    os << ";\n\n";
    return os;
}



bool CUnitUserOracleStubModule::addToFile(const std::string& fileName,
        LocationAnalyseResult& oldLocs) {
    return updateOracleFile<
            CUnitUserOracleStubDefinition,
            CUnitUserOracleStubModule>(*this, fileName, oldLocs);
}

bool CUnitUserOracleStubHeader::addToFile(const std::string& fileName,
        LocationAnalyseResult& oldLocs) {
    return updateOracleFile<
                CUnitUserOracleStubProto,
                CUnitUserOracleStubHeader>(*this, fileName, oldLocs);
}

std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubModule& module) {
    auto includes = util::getIncludesForFunctions(module.funcs.begin(), module.funcs.end(),
            module.prototypes);
    util::writeIncludes(includes.begin(), includes.end(), os, module.baseDirectory, module.moduleName);
    os << "\n";
    for (auto& f: module.funcs) {
        os << CUnitUserOracleStubDefinition(f, module.stp, module.mit);
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const CUnitUserOracleStubHeader& hdr) {
    auto includes = util::getIncludesForFunctions(hdr.funcs.begin(), hdr.funcs.end(),
            hdr.prototypes);
    util::writeIncludes(includes.begin(), includes.end(), os, hdr.baseDirectory, hdr.moduleName);
    os << "\n";
    auto includeGuard =  util::toUpperCase(TestDumpPass::oracleHeaderPath(hdr.moduleName));
    std::replace(includeGuard.begin(), includeGuard.end(), '.', '_');
    std::replace(includeGuard.begin(), includeGuard.end(), '/', '_');
    includeGuard = "_" + includeGuard + "_";
    os << "#ifndef " << includeGuard << "\n";
    os << "#define " << includeGuard << "\n\n";
    for (auto& f: hdr.funcs) {
        os << CUnitUserOracleStubProto(f, hdr.stp, hdr.mit);
    }
    os << "#endif /* " + includeGuard + " */\n";
    return os;
}

} /* namespace util */

} /* namespace borealis */
