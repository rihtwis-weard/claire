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

  IRCodeGenerator::IRCodeGenerator(std::string const &source_fname)
    : ctx_{}
    , mod_{"claire", ctx_}
    , builder_{ctx_}
    , machine_{set_target_machine(mod_)} {
    mod_.setSourceFileName(source_fname);
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

  llvm::Value *IRCodeGenerator::operator()(parser::ProgramDecl const *prog) {
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
  //
  //  llvm::Value *IRCodeGenerator::operator()(parser::ModuleDecl const *decl) {
  //    mod_fns_[decl->id()] = {};
  //
  //    for (auto const &child : decl->children()) {
  //      auto callee = std::visit(*this, child->as_variant());
  //
  //      // TODO(rihtwis-weard): nested visitors
  //      if (auto edecl = dynamic_cast<parser::ExternDecl const *>(child.get())) {
  //        mod_fns_[decl->id()][edecl->id()] = callee;
  //      }
  //    }
  //    return nullptr;
  //  }
  //
  //  llvm::Value *IRCodeGenerator::operator()(parser::ExternDecl const *decl) {
  //    // TODO(rihtwis-weard): `ExternDecl` allows generic typing and binding to any value,
  //    //                      does not assume function type signature
  //
  //    auto result = builder_.getVoidTy();
  //    if (decl->return_type() == "u32") {
  //      result = builder_.getInt32Ty();
  //    }
  //
  //    std::vector<llvm::Type *> params{};
  //
  //    for (auto const &arg : decl->args()) {
  //      if (arg.type == "binary") {
  //        params.push_back(builder_.getInt8PtrTy());
  //      } else {
  //        // TODO(rihtwis-weard): error handling because args don't matchup
  //      }
  //    }
  //
  //    auto fn = mod_.getOrInsertFunction(
  //      decl->linkage_name(), llvm::FunctionType::get(result, params, false));
  //    return fn.getCallee();
  //  }

  // TODO(rihtwis-weard): error-handling
  llvm::Value *IRCodeGenerator::operator()(parser::StringExpr const *expr) {
    return builder_.CreateGlobalStringPtr(expr->value());
  }

  llvm::Value *IRCodeGenerator::operator()(parser::FunctionCallExpr const *expr) {
    // TODO(rihtwis-weard): can/should separate modules be compiled as separate object files?
    //                      or always/conditionally be inlined as AST?

    // TODO(rihtwis-weard): nested visitors vs. if statements for function callee resolution?
    //    if (auto ma_expr = dynamic_cast<parser::ModuleAccessExpr const *>(expr->callee())) {
    //      auto value = mod_fns_.at(ma_expr->module_name()).at(ma_expr->id());
    //
    //      std::vector<llvm::Value *> args;
    //      for (auto const &child : expr->children()) {
    //        if (auto arg = std::visit(*this, child->as_variant()); arg) {
    //          args.push_back(arg);
    //        } else {
    //          std::cerr << "failed to create function arg!\n";
    //          return nullptr;
    //        }
    //      }
    //
    //      if (auto callee = mod_.getFunction(value->getName())) {
    //        return builder_.CreateCall(callee, args, "calltmp");
    //      } else {
    //        std::cerr << "unknown function referenced!\n";
    //        return nullptr;
    //      }
    //
    //    } else {
    //      std::cerr << "function callee type is unsupported or unknown!\n";
    //      return nullptr;
    //    }
    return nullptr;
  }

} // namespace claire::codegen
