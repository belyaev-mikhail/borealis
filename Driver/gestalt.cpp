/*
 * gestalt.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: belyaev
 */

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/Version.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/TextDiagnosticBuffer.h>
#include <clang/Frontend/Utils.h>
#include <clang/FrontendTool/Utils.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Parse/Parser.h>

#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/ADT/StringSet.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Analysis/CallGraph.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/RegionPass.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/CallGraphSCCPass.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/PassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/Program.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/SystemUtils.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Target/TargetLibraryInfo.h>
#include <llvm/Target/TargetMachine.h>

#include <google/protobuf/stubs/common.h>
#include <Passes/Defect/DefectManager.h>

#include "Actions/GatherCommentsAction.h"
#include "Config/config.h"
#include "Codegen/DiagnosticLogger.h"
#include "Driver/cl.h"
#include "Driver/clang_pipeline.h"
#include "Driver/gestalt.h"
#include "Driver/interviewer.h"
#include "Driver/llvm_pipeline.h"
#include "Driver/mpi_driver.h"
#include "Driver/plugin_loader.h"
#include "Logging/logger.hpp"
#include "Passes/Misc/PrinterPasses.h"
#include "Passes/Util/DataProvider.hpp"
#include "Util/util.h"

namespace borealis {
namespace driver {

static config::BoolConfigEntry compileOnly("run", "compileOnly");

int gestalt::main(int argc, const char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    atexit(google::protobuf::ShutdownProtobufLibrary);

    borealis::util::initFilePaths(argv);

    // XXX: sometimes this is causing "pure virtual method called" on sayonara
    // atexit(llvm::llvm_shutdown);

    using namespace clang;
    using namespace llvm;

    using namespace borealis::config;

    using borealis::endl;

    const std::vector<std::string> empty;

    CommandLine args(argc, argv);

    std::string configPath = "wrapper.conf";
    std::string defaultLogIni = "log.ini";
    auto realConfigPath = util::getFilePathIfExists(configPath);

    AppConfiguration::initialize(
        new CommandLineConfigSource{ args.suffixes("---").stlRep() },
        new FileConfigSource{ args.suffixes("---config:").single(realConfigPath.c_str()) }
    );

    CommandLine opt = CommandLine("wrapper") +
        args.suffixes("---opt:") +
        MultiConfigEntry("opt", "load").get();
    llvm::cl::ParseCommandLineOptions(opt.argc(), opt.argv());

    StringConfigEntry logFile("logging", "ini");
    StringConfigEntry z3log("logging", "z3log");


    borealis::logging::configureLoggingFacility(
        util::getFilePathIfExists(logFile.get().getOrElse(defaultLogIni))
    );

    for (const auto& op : z3log) {
        borealis::logging::configureZ3Log(util::getFilePathIfExists(op));
    }

    infos() << "Using config at " << realConfigPath << endl;

    auto prePasses = MultiConfigEntry("passes", "pre").get();
    auto inPasses = MultiConfigEntry("passes", "in").get();
    auto postPasses = MultiConfigEntry("passes", "post").get();
    auto libs = MultiConfigEntry("libs", "load").get();
    auto skipClang = BoolConfigEntry("run", "skipClangDriver").get(false);

    CommandLine compilerArgs = args.tail().unprefix("---");

    infos() << "Invoking clang with: " << compilerArgs << endl;

    auto diagOpts = llvm::IntrusiveRefCntPtr<DiagnosticOptions>(new DiagnosticOptions{});
    diagOpts->ShowCarets = false;

    auto diagBuffer = borealis::util::uniq(new DiagnosticLogger(*this));
    auto diags = CompilerInstance::createDiagnostics(diagOpts.get(), diagBuffer.get(), /* ownClient = */false);

    // first things first
    // fall-through to the regular clang

    auto nativeClangCfg = borealis::config::StringConfigEntry("run", "clangExec");

    interviewer nativeClang{ "clang", compilerArgs.data(), diags, nativeClangCfg };
    nativeClang.assignLogger(*this);

    auto compileCommands = nativeClang.getCompileCommands();

    auto linkIt = std::find_if(compileCommands.begin(), compileCommands.end(), [] (const command& cmd) {
        return cmd.operation == command::LINK;
    });

    if(linkIt != compileCommands.end() && compileOnly.get(false)) return OK;

    if (!skipClang) if (nativeClang.run() == interviewer::status::FAILURE) return E_CLANG_INVOKE;

    // prep for borealis business
    // compile sources to llvm::Module

    if (compileCommands.empty()) return E_ILLEGAL_COMPILER_OPTIONS;



    clang_pipeline clang { "clang" };

    clang.assignLogger(*this);
    clang.invoke(compileCommands);

    auto annotatedModule = clang.result();

    if (!annotatedModule) return OK;
    // TODO: Distinct between cases when we fucked up and when
    //       we were supposed to fuck up

    // collect passes
    auto module_ptr = annotatedModule->module;

    std::vector<StringRef> passes2run;
    passes2run.insert(passes2run.end(), prePasses.begin(), prePasses.end());
    passes2run.insert(passes2run.end(), inPasses.begin(), inPasses.end());

    std::vector<StringRef> libs2load;
    libs2load.insert(libs2load.end(), libs.begin(), libs.end());

    // Start up MPI
    MPI::Init();
    mpi::MPI_Driver driver{};

    // if we are root, print log
    if (driver.isRoot()) {
        // print list of pre passes
        borealis::logging::log_entry passes(infos());
        passes << "Passes:" << endl;
        if (passes2run.empty()) passes << "  " << "None" << endl;
        for (const auto& pass : passes2run) {
            passes << "  " << pass << endl;
        }

        // print list of dynamic libraries
        borealis::logging::log_entry libs(infos());
        libs << "Dynamic libraries:" << endl;
        if (libs2load.empty()) libs << "  " << "None" << endl;
        for (const auto& lib : libs2load) {
            libs << "  " << lib << endl;
        }
    }

    // load .so plugins
    plugin_loader pl;
    pl.assignLogger(*this);
    for (const auto& lib : libs2load) {
        pl.add(util::getFilePathIfExists(lib.str()));
    }
    pl.run();

    // run pre passes
    llvm_pipeline pre_pipeline { module_ptr };
    pre_pipeline.assignLogger(*this);

    pre_pipeline.add(*annotatedModule->annotations);
    pre_pipeline.add(annotatedModule->extVars);
    clang::FileManager files{ FileSystemOptions() };
    pre_pipeline.add(files);
    for (auto&& pass : passes2run) {
        pre_pipeline.add(pass.str());
    }

    pre_pipeline.run();

    // run post passes

    auto&& runPostPipeline = [&] (llvm::Function& function) {
        llvm_pipeline post_pipeline{module_ptr};
        post_pipeline.assignLogger(*this);
        post_pipeline.add(*annotatedModule->annotations);
        post_pipeline.add(annotatedModule->extVars);
        post_pipeline.add(files);
        for (auto&& pass : postPasses) {
            post_pipeline.add(pass);
        }
        // provide this function for PassModularizer using DataProviderPass
        post_pipeline.add(function);

        post_pipeline.run();
    };

    if (driver.isRoot()) {
        // verify we didn't screw up the module structure
        std::string err;
        llvm::raw_string_ostream rso{err};
        if (verifyModule(*module_ptr, &rso)) {
            errs() << "Module errors detected: " << rso.str() << endl;
        }

        // print list of post passes
        borealis::logging::log_entry out(infos());
        out << "Post passes:" << endl;
        if (postPasses.empty()) out << "  " << "None" << endl;
        for (const auto& pass : postPasses) {
            out << "  " << pass << endl;
        }
    }

    // if we run without mpi
    if (not driver.isMPI()) {
        DefectManager::initAdditionalDefectData();

        util::viewContainer(*module_ptr)
                .foreach(runPostPipeline);

        DefectManager::dumpPersistentDefectData();
        MPI::Finalize();
        return OK;
    }

#include "Util/macros.h"

    std::vector<llvm::Function*> functions;
    util::viewContainer(*module_ptr).foreach(
        [&functions](llvm::Function& function) {
            if (not function.isDeclaration())
                functions.push_back(&function);
        }
    );

    std::sort(functions.begin(), functions.end(), [] (llvm::Function* a, llvm::Function* b) {
        return a->getName() < b->getName();
    });

    // produсer process
    if (driver.isRoot()) {

        auto numOfFreeProc = driver.getSize() - 1;
        auto function = functions.begin();

        //process all functions of module
        while (true) {
            driver.receive();
            auto status = driver.getStatus();
            ASSERT(status.tag_ == mpi::Tag::READY, "Unexpected message received by root")

            // if we still have function to analyze, send it to consumer
            if (function != functions.end()) {
                // sending a message to consumer
                infos() << "Function " << (function - functions.begin() + 1) << " out of " << functions.size() << endl;
                driver.send(status.source_, { int(function - functions.begin()), mpi::Tag::FUNCTION });
                ++function;

            // else just terminate consumer
            } else {
                driver.terminate(status.source_);
                // if there are no more consumers, terminate itself
                if ( (--numOfFreeProc) == 0 ) break;
            }
        }

    // consumer processes
    } else {

        DefectManager::initAdditionalDefectData();
        while (true) {
            driver.send(mpi::Rank::ROOT, { mpi::MPI_Driver::ANY, mpi::Tag::READY });
            auto functionIndex = (size_t) driver.receive(mpi::Rank::ROOT).getData();
            auto status = driver.getStatus();

            // stop work if producer tells us to stop
            if (status.tag_ == mpi::Tag::TERMINATE) break;

            //check that we received correct message
            ASSERT(status.tag_ == mpi::Tag::FUNCTION, "Unexpected message received by consumer");
            ASSERT(functionIndex >= 0 && functionIndex < functions.size(), "Incorrect function index received by consumer");

            // analyze function
            infos() << driver.getRank() << " started function " << functions[functionIndex]->getName() << endl;
            runPostPipeline(*functions[functionIndex]);
            infos() << driver.getRank() << " finished function " << functions[functionIndex]->getName() << endl;
        }
        DefectManager::dumpPersistentDefectData();

    }

    MPI::Finalize();

#include "Util/unmacros.h"

    return OK;
}

gestalt::~gestalt() {}

} /* namespace driver */
} /* namespace borealis */
