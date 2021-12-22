#include <iostream>

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "ast.hpp"

namespace {
  llvm::LLVMContext             context{};
  llvm::IRBuilder<>             builder{context};
  std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("main", context);
  std::unordered_map<std::string, llvm::Value *> named_values{};
} // namespace

namespace claire {
  llvm::Value *log_error(char const *err) {
    std::cerr << err << "\n";
    return nullptr;
  }

  llvm::Value *StringExpr::codegen() {
    //    module->getOrInsertGlobal(name_, builder.getIntPtrTy(module->getDataLayout()));
    return builder.CreateGlobalStringPtr(name_);
  }

  llvm::Value *FunctionCallExpr::codegen() {
    auto callee = module->getFunction("puts");
    if (callee) {
      return log_error("unknown function referenced");
    }

    std::vector<llvm::Value *> args;
    for (auto const &child : children_) {
      args.push_back(child->codegen());
    }

    return builder.CreateCall(callee, args, "calltmp");
  }

} // namespace claire
