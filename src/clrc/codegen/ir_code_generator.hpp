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
#include <robin_hood.h>

#include "../parser/ast.hpp"

namespace claire::codegen {

  class IRCodeGenerator : public parser::ASTVisitor {
    llvm::LLVMContext    ctx_;
    llvm::Module         mod_;
    llvm::IRBuilder<>    builder_;
    llvm::TargetMachine *machine_;

    robin_hood::unordered_map<std::string,
      robin_hood::unordered_map<std::string, llvm::Value *>>
      mod_fns_;

  public:
    explicit IRCodeGenerator(std::string const &source_fname);

    using ASTVisitor::operator();

    std::string dumps() const;

    void emit_object_code();

    llvm::Value *operator()(parser::ProgramDecl const *) override;
    llvm::Value *operator()(parser::ModuleDecl const *) override;
    llvm::Value *operator()(parser::ExternDecl const *) override;
    llvm::Value *operator()(parser::StringExpr const *) override;
    llvm::Value *operator()(parser::FunctionCallExpr const *) override;

    llvm::Value *operator()(parser::IdentifierExpr const *) override {
      return nullptr;
    }
  };
} // namespace claire::codegen
