#include "ir_code_generator.hpp"

#include <iostream>

namespace claire::codegen {

  llvm::TargetMachine *set_target_machine(llvm::Module &mod) {
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

    mod.setDataLayout(target_machine->createDataLayout());
    mod.setTargetTriple(target_triple);
    return target_machine;
  }

  // TODO(rihtwis-weard): need to move to standard library module + AST instead of hard-coding
  void hardcoded_ffi(llvm::Module &mod, llvm::IRBuilder<> &builder) {
    // int puts(char const *s);
    mod.getOrInsertFunction("puts",
      llvm::FunctionType::get(builder.getInt32Ty(), builder.getInt8PtrTy(), false));
  }

  IRCodeGenerator::IRCodeGenerator(std::string const &source_fname)
    : ctx_{}
    , mod_{"claire", ctx_}
    , builder_{ctx_}
    , machine_{set_target_machine(mod_)} {
    mod_.setSourceFileName(source_fname);

    hardcoded_ffi(mod_, builder_);
  }

  std::string IRCodeGenerator::dumps() const {
    auto str = std::string{};
    auto ss  = llvm::raw_string_ostream{str};
    mod_.print(ss, nullptr);
    return ss.str();
  }

  void IRCodeGenerator::emit_object_code() {
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

  //-----------------------------------------------------------------------------------------------
  // Visitors
  //-----------------------------------------------------------------------------------------------

  llvm::Value *IRCodeGenerator::operator()(ProgramDecl const *prog) {
    auto *main_ft = llvm::FunctionType::get(builder_.getInt32Ty(), false);
    auto *main =
      llvm::Function::Create(main_ft, llvm::Function::ExternalLinkage, "main", mod_);

    builder_.SetInsertPoint(llvm::BasicBlock::Create(ctx_, "entry", main));

    for (auto const &child : prog->children()) {
      std::visit(*this, child->as_variant());
    }

    constexpr auto int_size  = 32;
    constexpr auto is_signed = true;

    auto ret = llvm::APInt{int_size, 0, is_signed};
    return builder_.CreateRet(llvm::ConstantInt::get(ctx_, ret));
  }

  // TODO(rihtwis-weard): error-handling
  llvm::Value *IRCodeGenerator::operator()(StringExpr const *expr) {
    return builder_.CreateGlobalStringPtr(expr->name());
  }

  llvm::Value *IRCodeGenerator::operator()(FunctionCallExpr const *expr) {
    auto callee = mod_.getFunction("puts");
    if (not callee) {
      std::cerr << "unknown function referenced!\n";
      return nullptr;
    }

    std::vector<llvm::Value *> args;
    for (auto const &child : expr->children()) {
      if (auto arg = std::visit(*this, child->as_variant()); arg) {
        args.push_back(arg);
      } else {
        std::cerr << "failed to create function arg!\n";
        return nullptr;
      }
    }

    return builder_.CreateCall(callee, args, "calltmp");
  }

} // namespace claire::codegen
