cmake_minimum_required(VERSION 3.2)
project(borealis)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

find_package(LLVM QUIET REQUIRED COMPONENTS ALL
        CONFIG
        PATHS /opt/clang/3.5.1/share/llvm/cmake/
        NO_DEFAULT_PATH)

link_directories(${LLVM_LIBRARY_DIRS})
link_directories(/usr/lib)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

if (NOT ${LLVM_FOUND})
    message(FATAL_ERROR "LLVM libraries not found")
endif ()

################################################################################
# warnings
################################################################################
set(WARNINGS_ON all extra cast-qual float-equal switch
        undef init-self pointer-arith cast-align effc++
        strict-prototypes strict-overflow=5 write-strings
        aggregate-return super-class-method-mismatch)

set(WARNINGS_OFF unused-function redundant-decls unused-local-typedef pessimizing-move)

set(WARNINGS_TAE overloaded-virtual return-stack-address
        implicit-function-declaration address-of-temporary
        delete-non-virtual-dtor return-type)

if (${CMAKE_CXX_COMPILER} STREQUAL "/usr/bin/clang++")
    foreach(w ${WARNINGS_ON})
        list(APPEND COMPILER_WARNINGS -W${w})
    endforeach()
    foreach(w ${WARNINGS_OFF})
        list(APPEND COMPILER_WARNINGS -Wno-${w})
    endforeach()
    foreach(w ${WARNINGS_TAE})
        list(APPEND COMPILER_WARNINGS -Werror-${w})
    endforeach()
endif ()

################################################################################
# compiler flags
################################################################################
add_definitions(${LLVM_DEFINITIONS}
        -std=c++14)

set(COMPILER_DEFINITIONS
        -D_GLIBCXX_USE_CXX11_ABI=1
        -DGOOGLE_PROTOBUF_NO_RTTI
        -DNO_TRACING)

set(COMPILER_FLAGS
        ${CMAKE_CXX_FLAGS}
        ${LLVM_CXX_FLAGS}
        ${LLVM_LD_FLAGS}
        -fuse-ld=gold
        -rdynamic
        -O0
        -fPIC
        -fno-rtti
        ${COMPILER_WARNINGS})

################################################################################
# include directories
################################################################################
set(INCLUDE_DIRS
        ${PROJECT_SOURCE_DIR}/Protobuf/Gen
        ${PROJECT_SOURCE_DIR}/lib
        ${PROJECT_SOURCE_DIR}/lib/pegtl/include
        ${PROJECT_SOURCE_DIR}/lib/google-test/include
        ${PROJECT_SOURCE_DIR}/lib/yaml-cpp/include
        ${PROJECT_SOURCE_DIR}/lib/yaml-cpp/test
        ${PROJECT_SOURCE_DIR}/lib/cfgparser/include
        ${PROJECT_SOURCE_DIR}/lib/backward-cpp)

include_directories(${PROJECT_SOURCE_DIR})

include_directories(SYSTEM /usr/include/z3)             # using SYSTEM parameter to exclude
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})        # compiler warnings on include libraries
include_directories(SYSTEM ${INCLUDE_DIRS})

################################################################################
# source directories
################################################################################

set(SOURCE_DIRS
#       ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/Actions
        ${PROJECT_SOURCE_DIR}/Anno
        ${PROJECT_SOURCE_DIR}/Annotation
        ${PROJECT_SOURCE_DIR}/Codegen
        ${PROJECT_SOURCE_DIR}/Config
        ${PROJECT_SOURCE_DIR}/Driver
        ${PROJECT_SOURCE_DIR}/Executor
        ${PROJECT_SOURCE_DIR}/Factory
        ${PROJECT_SOURCE_DIR}/Logging
        ${PROJECT_SOURCE_DIR}/Passes
        ${PROJECT_SOURCE_DIR}/Predicate
        ${PROJECT_SOURCE_DIR}/Protobuf
        ${PROJECT_SOURCE_DIR}/Reanimator
        ${PROJECT_SOURCE_DIR}/SMT
        ${PROJECT_SOURCE_DIR}/State
        ${PROJECT_SOURCE_DIR}/Statistics
        ${PROJECT_SOURCE_DIR}/Term
        ${PROJECT_SOURCE_DIR}/Type
        ${PROJECT_SOURCE_DIR}/Util)

foreach(dir ${SOURCE_DIRS})
    file(GLOB_RECURSE SRC ${dir}/*.cc
                  ${dir}/*.cpp)
    foreach(file ${SRC})
        list(APPEND SOURCES "${file}")
    endforeach()
endforeach()

add_library(SourceObjects OBJECT ${SOURCES})

################################################################################
# Test sources
################################################################################
set(TEST_DIRS ${PROJECT_SOURCE_DIR}/test)

foreach(dir ${TEST_DIRS})
    file(GLOB_RECURSE SRC ${dir}/*.cpp)
    foreach(file ${SRC})
        list(APPEND TEST_SOURCES "${file}")
    endforeach()
endforeach()

add_library(TestObjects OBJECT ${TEST_SOURCES})

################################################################################
# Google Test
################################################################################
set(GOOGLE_TEST_DIR ${PROJECT_SOURCE_DIR}/lib/google-test)

add_subdirectory(${GOOGLE_TEST_DIR})

link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/google-test)
list(APPEND TEST_ARCHIVES gtest)

################################################################################
# yaml-cpp
################################################################################
set(YAML_CPP_DIR ${PROJECT_SOURCE_DIR}/lib/yaml-cpp)

add_subdirectory(${YAML_CPP_DIR})

link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/yaml-cpp)
list(APPEND ARCHIVES yaml-cpp)

################################################################################
# dbglog
################################################################################
# cfgparser needs dbglog, but we don't
set(DBGLOG_DIR ${PROJECT_SOURCE_DIR}/lib/dbglog)
set(DBGLOG_DIST ${DBGLOG_DIR}/dist)
set(DBGLOG_LIB ${DBGLOG_DIST}/lib/)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/.dbglog
        COMMAND ./configure CXX=${CMAKE_CXX_COMPILER} --prefix=${DBGLOG_DIST}
        COMMAND make CXX=${CMAKE_CXX_COMPILER} -C ${DBGLOG_DIR} install
        COMMAND touch ${PROJECT_SOURCE_DIR}/.dbglog
        WORKING_DIRECTORY ${DBGLOG_DIR})

add_custom_target(.dbglog
        DEPENDS ${PROJECT_SOURCE_DIR}/.dbglog)

add_custom_target(clean.dbglog
        COMMAND rm ${PROJECT_SOURCE_DIR}/.dbglog
        COMMAND make -C ${DBGLOG_DIR} clean
        COMMAND make -C ${DBGLOG_DIR} distclean
        COMMAND rm -rf ${DBGLOG_DIST})

link_directories(${DBGLOG_LIB})
list(APPEND ARCHIVES dbglog)

################################################################################
# cfgparser
################################################################################
set(CFGPARSER_DIR ${PROJECT_SOURCE_DIR}/lib/cfgparser)
set(CFGPARSER_LIB ${CFGPARSER_DIR}/dist/lib/)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/.cfgparser
        COMMAND ./configure --prefix=${CFGPARSER_DIR}/dist --with-dbglog=${DBGLOG_DIST}
        COMMAND make CXX=${CMAKE_CXX_COMPILER} -C ${CFGPARSER_DIR} install
        COMMAND touch ${PROJECT_SOURCE_DIR}/.cfgparser
        WORKING_DIRECTORY ${CFGPARSER_DIR})

add_custom_target(.cfgparser
        DEPENDS ${PROJECT_SOURCE_DIR}/.cfgparser)

add_custom_target(clean.cfgparser
        COMMAND rm ${PROJECT_SOURCE_DIR}/.cfgparser
        COMMAND make CXX=${CMAKE_CXX_COMPILER} -C ${CFGPARSER_DIR} clean
        COMMAND make CXX=${CMAKE_CXX_COMPILER} -C ${CFGPARSER_DIR} distclean
        COMMAND rm -rf ${CFGPARSER_DIR}/dist)

#dependencies with dbglog
add_dependencies(.cfgparser .dbglog)
add_dependencies(clean.cfgparser clean.dbglog)

link_directories(${CFGPARSER_LIB})
list(APPEND ARCHIVES cfgparser)

################################################################################
# andersen
################################################################################
set(ANDERSEN_CPP_DIR ${PROJECT_SOURCE_DIR}/lib/andersen)

add_subdirectory(${ANDERSEN_CPP_DIR})

link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/andersen/lib)
list(APPEND ARCHIVES libAndersen.so)

################################################################################
# Valgrind
################################################################################
set(VALGRIND "valgrind --leak-check=yes --suppressions=valgrind.supp")

################################################################################
# Protobuf
################################################################################
#Don't know how to fucking generate CPP files in required directory
set(PROTO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Protobuf/Gen)
set(PROTOC protoc --proto_path=${PROJECT_SOURCE_DIR} --cpp_out=${PROTO_SOURCE_DIR})
set(PROTOEXT ${PROJECT_SOURCE_DIR}/extract-protobuf-desc.awk)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/.protobuf
        COMMAND mkdir -p ${PROTO_SOURCE_DIR}
        COMMAND ${PROTOEXT} `find ${PROJECT_SOURCE_DIR} -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | sort`
        COMMAND find ${PROJECT_SOURCE_DIR} -name "*.proto" | xargs ${PROTOC}
        COMMAND touch ${PROJECT_SOURCE_DIR}/.protobuf
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_custom_target(.protobuf
        DEPENDS ${PROJECT_SOURCE_DIR}/.protobuf)

add_custom_target(clean.protobuf
        COMMAND rm ${PROJECT_SOURCE_DIR}/.protobuf
        COMMAND find ${PROJECT_SOURCE_DIR} -name "*.proto" -delete
        COMMAND rm -rf ${PROTO_SOURCE_DIR})

################################################################################
# libraries
################################################################################
set(SYSTEM_LIBS
        z3
        log4cpp
        profiler
        jsoncpp
        protobuf
        tinyxml2
        mathsat
        gmpxx
        gmp
        uuid
        stdc++
        rt
        dl
        curses
        pthread
        z
        m)

set(CLANG_LIBS
        clangFrontendTool
        clangFrontend
        clangDriver
        clangSerialization
        clangCodeGen
        clangParse
        clangSema
        clangStaticAnalyzerFrontend
        clangStaticAnalyzerCheckers
        clangStaticAnalyzerCore
        clangAnalysis
        clangARCMigrate
        clangRewrite
        clangEdit
        clangAST
        clangLex
        clangBasic)

################################################################################
# executables
################################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# wrapper
add_executable(wrapper wrapper.cpp $<TARGET_OBJECTS:SourceObjects>)

target_compile_options(wrapper PUBLIC ${COMPILER_FLAGS})
target_compile_definitions(wrapper PUBLIC ${COMPILER_DEFINITIONS})

add_dependencies(wrapper
        .protobuf
        yaml-cpp
        .cfgparser
        Andersen)

target_link_libraries(wrapper ${ARCHIVES})
target_link_libraries(wrapper ${SYSTEM_LIBS} ${CLANG_LIBS} ${LLVM_AVAILABLE_LIBS})

#ar_wrapper
add_executable(ar_wrapper ar_wrapper.cpp  $<TARGET_OBJECTS:SourceObjects>)

target_compile_options(ar_wrapper PUBLIC ${COMPILER_FLAGS})
target_compile_definitions(ar_wrapper PUBLIC ${COMPILER_DEFINITIONS})

add_dependencies(ar_wrapper
        .protobuf
        yaml-cpp
        .cfgparser
        Andersen)

target_link_libraries(ar_wrapper ${ARCHIVES})
target_link_libraries(ar_wrapper ${SYSTEM_LIBS} ${CLANG_LIBS} ${LLVM_AVAILABLE_LIBS})

################################################################################
# test executable
################################################################################
set(TEST_OUTPUT test_results.xml)
set(TEST_EXE wrapper-tests)
set(RUN_TEST_EXES ${PROJECT_SOURCE_DIR}/${TEST_EXE} --gtest_output="xml:${TEST_OUTPUT}" --gtest_color=yes)

add_executable(${TEST_EXE} EXCLUDE_FROM_ALL $<TARGET_OBJECTS:SourceObjects> $<TARGET_OBJECTS:TestObjects>)
add_dependencies(${TEST_EXE}
        .protobuf
        yaml-cpp
        .cfgparser
        Andersen
        gtest)

target_compile_options(${TEST_EXE} PUBLIC ${COMPILER_FLAGS})
target_compile_definitions(${TEST_EXE} PUBLIC ${COMPILER_DEFINITIONS})

target_link_libraries(${TEST_EXE} ${ARCHIVES} ${TEST_ARCHIVES})
target_link_libraries(${TEST_EXE} ${SYSTEM_LIBS} ${CLANG_LIBS} ${LLVM_AVAILABLE_LIBS})

################################################################################
# testing targets
################################################################################

foreach(dir ${TEST_DIRS})
    execute_process(COMMAND "ls -1 ${dir} | grep '^.*.c$' | xargs -n1 basename > tests.def.all")
    execute_process(COMMAND "sed -e 's|^#\\s*||' ${dir}/tests.def.long > tests.def.long.all")
    execute_process(COMMAND "grep -v -x -f ${dir}.long.all tests.def.all > tests.def")
endforeach()

add_custom_target(.regenerate-test-defs
        COMMAND find ${TEST_DIRS} -name "*.tmp" -delete)

add_custom_target(check
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ${RUN_TEST_EXES} --gtest_filter=-*Long/*:-*Summary* || true)
add_dependencies(check
        .regenerate-test-defs
        ${TEST_EXE})

add_custom_target(check-with-valgrind
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ${VALGRIND} ${RUN_TEST_EXES} --gtest_filter=-*Long/*:-*Summary* || true)
add_dependencies(check-with-valgrind
        .regenerate-test-defs
        ${TEST_EXE})

add_custom_target(check-long
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ${RUN_TEST_EXES} --gtest_filter=*Long/*:-*Summary* || true)
add_dependencies(check-long
        .regenerate-test-defs
        ${TEST_EXE})

add_custom_target(check-summary
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ${RUN_TEST_EXES} --gtest_filter=*Summary*:-*Long/* || true)
add_dependencies(check-summary
        .regenerate-test-defs
        ${TEST_EXE})

add_custom_target(check-summary-long
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ${RUN_TEST_EXES} --gtest_filter=*SummaryLong/* || true)
add_dependencies(check-summary-long
        .regenerate-test-defs
        ${TEST_EXE})

add_custom_target(check-all
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMAND ${RUN_TEST_EXES} --gtest_filter=-*Summary* || true)
add_dependencies(check-all
        .regenerate-test-defs
        ${TEST_EXE})
