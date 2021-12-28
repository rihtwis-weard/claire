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

    builder_.SetInsertPoint(make_entrypoint());
  }

  llvm::BasicBlock *IRCodeGenerator::make_entrypoint() {
    auto *main_ft = llvm::FunctionType::get(builder_.getInt32Ty(), false);
    auto *main =
      llvm::Function::Create(main_ft, llvm::Function::ExternalLinkage, "main", mod_);
    return llvm::BasicBlock::Create(ctx_, "entry", main);
  }

  void IRCodeGenerator::finish_program() {
    constexpr auto int_size  = 32;
    constexpr auto is_signed = true;

    auto ret = llvm::APInt{int_size, 0, is_signed};
    builder_.CreateRet(llvm::ConstantInt::get(ctx_, ret));
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
  // TODO(rihtwis-weard): error-handling
  void IRCodeGenerator::operator()(StringExpr const *expr) {
    if (auto val = builder_.CreateGlobalStringPtr(expr->name()); not val) {
      std::cerr << "failed to create constant string\n";
    }
  }

  void IRCodeGenerator::operator()(FunctionCallExpr const *expr) {
    auto callee = mod_.getFunction("puts");
    if (not callee) {
      std::cerr << "unknown function referenced!\n";
      return;
    }

    std::vector<llvm::Value *> args;
    for (auto const &child : expr->children()) {
      args.push_back(child->codegen(mod_, builder_));
    }

    if (auto val = builder_.CreateCall(callee, args, "calltmp"); not val) {
      std::cerr << "failed to create function call\n";
    }
  }

} // namespace claire::codegen
