#include "../ast.hpp"

namespace claire {

  void ASTNode::traverse(ASTVisitor &v) const {
    std::visit(v, as_variant());
    for (auto const &child : children_) {
      child->traverse(v);
    }
  }
} // namespace claire
