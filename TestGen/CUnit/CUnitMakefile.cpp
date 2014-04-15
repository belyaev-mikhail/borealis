/* 
 * File:   CUnitMakefile.cpp
 * Author: maxim
 * 
 * Created on 11 Апрель 2014 г., 15:55
 */

#include "Passes/TestGeneration/TestDumpPass.h"
#include "TestGen/CUnit/CUnitMakefile.h"
#include "Util/filename_utils.h"

namespace borealis {
namespace util {
    
std::ostream& operator<<(std::ostream& os, const CUnitMakefile& makefile) {
    os << "CC = clang\n\n";
    if (TestDumpPass::includeInMakefile()) {
        os << "CFLAGS = -I";
        if (TestDumpPass::absoluteInclude()) {
            os << makefile.baseDirectory;
        } else {
            os << util::getRelativePath(makefile.baseDirectory, "", makefile.name);
        }
        os << "\n\n";
    }
    os << "LDFLAGS = -lcunit\n\n";
    
    os << "SOURCES =";
    for (const auto& s: makefile.sources) {
        os << " " << util::getRelativePath(makefile.baseDirectory,
                    llvm::StringRef(s),
                    llvm::StringRef(makefile.name));
    }
    os << "\n\n";
    
    os << "TESTS =";
    for (const auto& t: makefile.tests) {
        os << " " << util::getRelativePath(makefile.baseDirectory,
                    llvm::StringRef(t),
                    llvm::StringRef(makefile.name));
    }
    os << "\n\n";
    
    os << "ORACLES =";
    for (const auto& o: makefile.oracles) {
        os << " " << util::getRelativePath(makefile.baseDirectory,
                    llvm::StringRef(o),
                    llvm::StringRef(makefile.name));
    }
    os << "\n\n";
    
    os << "SOURCE_OBJECTS = $(SOURCES:.c=.o)\n\n";
    os << "TEST_OBJECTS = $(TESTS:.c=.o)\n\n";
    os << "ORACLE_OBJECTS = $(ORACLES:.c=.o)\n\n";
    
    os << "EXEC = test\n\n";
    
    os << "$(EXEC): $(SOURCE_OBJECTS) $(TEST_OBJECTS) $(ORACLE_OBJECTS)\n";
    os << "\t$(CC) $(LDFLAGS) -o $(EXEC) $^\n";
    
    os << "check: $(EXEC)\n";
    os << "\t./$(EXEC)\n";
    
    os << "clean:\n";
    os << "\trm -f $(TEST_OBJECTS) $(ORACLE_OBJECTS) $(EXEC)\n";

    os << "clean-all:\n";
    os << "\trm -f $(SOURCE_OBJECTS) $(TEST_OBJECTS) $(ORACLE_OBJECTS) $(EXEC)\n";
    return os;
}

} /* namespace util */
} /* namespace borealis */