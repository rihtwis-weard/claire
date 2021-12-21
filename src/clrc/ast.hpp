#pragma once

#include <memory>
#include <utility>

namespace claire {
  class ASTNode {

    std::vector<std::unique_ptr<ASTNode>> children_;

  public:
    void add(std::unique_ptr<ASTNode> &&node) {
      children_.emplace_back(std::move(node));
    }

    void add(std::vector<std::unique_ptr<ASTNode>> &&nodes) {
      children_.insert(children_.end(), std::make_move_iterator(nodes.begin()),
        std::make_move_iterator(nodes.end()));
    }
  };

  class Expr : public ASTNode {
    std::string name_;

  public:
    explicit Expr(std::string name)
      : name_{std::move(name)} {
    }
  };

} // namespace claire
