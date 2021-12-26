#include <iostream>

#include <llvm/IR/Constants.h>

#include "codegen.hpp"

namespace claire {
  llvm::Value *log_error(char const *err) {
    std::cerr << err << "\n";
    return nullptr;
  }

  // TODO(rihtwis-weard): use visitor pattern instead of member function
  llvm::Value *StringExpr::codegen(llvm::Module &module, llvm::IRBuilder<> &builder) {
    return builder.CreateGlobalStringPtr(name_);
  }

  llvm::Value *FunctionCallExpr::codegen(
    llvm::Module &module, llvm::IRBuilder<> &builder) {
    auto callee = module.getFunction("puts");
    if (not callee) {
      return log_error("unknown function referenced");
    }

    std::vector<llvm::Value *> args;
    for (auto const &child : children_) {
      args.push_back(child->codegen(module, builder));
    }

    return builder.CreateCall(callee, args, "calltmp");
  }

} // namespace claire
