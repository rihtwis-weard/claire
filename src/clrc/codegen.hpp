#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include "ast.hpp"

namespace claire {
  class CodeGenerator {
    llvm::LLVMContext    ctx_;
    llvm::Module         mod_;
    llvm::IRBuilder<>    builder_;
    llvm::TargetMachine *machine_;

  public:
    explicit CodeGenerator(std::string const &source_fname)
      : ctx_{}
      , mod_{"claire", ctx_}
      , builder_{ctx_}
      , machine_{set_target_triple()} {
      mod_.setSourceFileName(source_fname);

      hardcoded_ffi();
      builder_.SetInsertPoint(create_entrypoint());
    }

    void codegen(ASTNode *node) {
      node->codegen(mod_, builder_);
      for (auto const &child : node->children_) {
        codegen(child.get());
      }
    }

    void fin() {
      constexpr auto int_size  = 32;
      constexpr auto is_signed = true;

      auto ret = llvm::APInt{int_size, 0, is_signed};
      builder_.CreateRet(llvm::ConstantInt::get(ctx_, ret));
    }

    std::string dumps() const {
      auto str = std::string{};
      auto ss  = llvm::raw_string_ostream{str};
      mod_.print(ss, nullptr);
      return ss.str();
    }

    void emit_object_code() {
      auto                 obj_fname = "hello_world.o";
      std::error_code      ec;
      llvm::raw_fd_ostream dest{obj_fname, ec, llvm::sys::fs::OpenFlags::OF_None};

      llvm::legacy::PassManager pass{};
      auto                      file_type = llvm::CodeGenFileType::CGFT_ObjectFile;

      if (machine_->addPassesToEmitFile(pass, dest, nullptr, file_type)) {
        std::exit(1); // TODO(rihtwis-weard): use exception
      }

      pass.run(mod_);
      dest.flush();
    }

  private:
    llvm::TargetMachine *set_target_triple() {
      std::string error{};

      llvm::InitializeAllTargetInfos();
      llvm::InitializeAllTargets();
      llvm::InitializeAllTargetMCs();
      llvm::InitializeAllAsmParsers();
      llvm::InitializeAllAsmPrinters();

      auto target_triple = llvm::sys::getDefaultTargetTriple();
      auto target        = llvm::TargetRegistry::lookupTarget(target_triple, error);

      if (not target) {
        std::cerr << error << "\n";
        std::exit(1); // TODO(rihtwis-weard): throw exception
      }

      constexpr auto cpu      = "generic";
      constexpr auto features = "";
      auto           opts     = llvm::TargetOptions{};
      auto           rm = llvm::Optional<llvm::Reloc::Model>{llvm::Reloc::Model::PIC_};
      auto           target_machine =
        target->createTargetMachine(target_triple, cpu, features, opts, rm);

      mod_.setDataLayout(target_machine->createDataLayout());
      mod_.setTargetTriple(target_triple);

      return target_machine;
    }

    llvm::BasicBlock *create_entrypoint() {
      auto *main_ft = llvm::FunctionType::get(builder_.getInt32Ty(), false);
      auto *main =
        llvm::Function::Create(main_ft, llvm::Function::ExternalLinkage, "main", mod_);
      return llvm::BasicBlock::Create(ctx_, "entry", main);
    }

    // TODO(rihtwis-weard): need to move to AST instead of hard-coding
    void hardcoded_ffi() {
      // int puts(char const *s);
      mod_.getOrInsertFunction("puts",
        llvm::FunctionType::get(builder_.getInt32Ty(), builder_.getInt8PtrTy(), false));
    }
  };
} // namespace claire
