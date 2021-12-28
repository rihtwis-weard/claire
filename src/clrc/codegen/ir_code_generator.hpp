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

#include "../ast.hpp"

namespace claire::codegen {

  class IRCodeGenerator : public ASTVisitor {
    llvm::LLVMContext    ctx_;
    llvm::Module         mod_;
    llvm::IRBuilder<>    builder_;
    llvm::TargetMachine *machine_;

  public:
    explicit IRCodeGenerator(std::string const &source_fname);

    using ASTVisitor::operator();

    std::string dumps() const;

    void emit_object_code();

    llvm::Value *operator()(ProgramDecl const *) override;
    llvm::Value *operator()(StringExpr const *) override;
    llvm::Value *operator()(FunctionCallExpr const *) override;

    llvm::Value *operator()(IdentifierExpr const *) override {
      return nullptr;
    }

    llvm::Value *operator()(AccessExpr const *) override {
      return nullptr;
    }
  };
} // namespace claire::codegen
