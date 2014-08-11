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
    
std::string ldflags() {
    static config::ConfigEntry<std::string> ldflags("testgen", "make-ldflags");
    return ldflags.get("");
}

std::string cflags() {
    static config::ConfigEntry<std::string> cflags("testgen", "make-cflags");
    return cflags.get("");
}

std::ostream& operator<<(std::ostream& os, const CUnitMakefile& makefile) {
    os << "CC = clang\n\n";
    
    if (TestDumpPass::includeInMakefile()) {
        os << "INCLUDE_DIRS =";
        if (TestDumpPass::absoluteInclude()) {
            os << " " << makefile.baseDirectory;
            os << " " << util::getAbsolutePath(makefile.baseDirectory, TestDumpPass::oracleDirectory());
        } else {
            os << " " << util::getRelativePath(makefile.baseDirectory, "", makefile.name);
            os << " " << util::getRelativePath(makefile.baseDirectory, TestDumpPass::oracleDirectory(), makefile.name);
        }
        os << "\n\n";
        
        os << "INCLUDES := $(foreach dir, $(INCLUDE_DIRS), -I\"$(dir)\")\n\n";

        os << "TESTCFLAGS = $(INCLUDES)\n\n";

        os << "CFLAGS = " << cflags() << "\n\n";
    }
    
    os << "TESTLDFLAGS = -lcunit\n\n";
    
    os << "LDFLAGS = " << ldflags() << "\n\n";

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

    os << "%.o: %.c\n";
    os << "\t$(CC) $(CFLAGS) $(TESTCFLAGS) -c $^ -o $@\n\n";
    
    os << "$(EXEC): $(SOURCE_OBJECTS) $(TEST_OBJECTS) $(ORACLE_OBJECTS)\n";
    os << "\t$(CC) $(LDFLAGS) $(TESTLDFLAGS) -o $(EXEC) $^\n";
    
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
