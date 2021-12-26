#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/Host.h>

#include "ast.hpp"

namespace claire {
  class CodeGenerator {
    llvm::LLVMContext ctx_;
    llvm::Module      mod_;
    llvm::IRBuilder<> builder_;

  public:
    explicit CodeGenerator(std::string const &source_fname)
      : ctx_{}
      , mod_{"claire", ctx_}
      , builder_{ctx_} {
      mod_.setSourceFileName(source_fname);
      set_target_triple();
    }

    void codegen(ASTNode *node) {
      builder_.SetInsertPoint(create_entrypoint());
      //
      //      node->codegen(mod_, builder_);
      //      for (auto const &child : node->children_) {
      //        codegen(child.get());
      //      }
      mod_.print(llvm::outs(), nullptr);
    }

  private:
    void set_target_triple() {
      mod_.setTargetTriple(llvm::sys::getDefaultTargetTriple());
    }

    llvm::BasicBlock *create_entrypoint() {
      auto *main_ft = llvm::FunctionType::get(builder_.getInt32Ty(), false);
      auto *main =
        llvm::Function::Create(main_ft, llvm::Function::ExternalLinkage, "main", mod_);
      return llvm::BasicBlock::Create(ctx_, "entry", main);
    }
  };
} // namespace claire
