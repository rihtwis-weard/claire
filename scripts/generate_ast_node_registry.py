#!/usr/bin/env python3

AST_NODE_TYPES = [
    "ASTNode",
    "ProgramDecl",
    "IdentifierExpr",
    "StringExpr",
    "FunctionCallExpr",
    "IdentifierSeq",
    "ExpressionSequence",
    "NamespaceAccessExpr",
]

CODE_TEMPLATE = """
// This code is auto-generated by generate_ast_node_registry.py, do not manually modify!!!
#pragma once
#include <variant>

namespace claire::parser {{
{}

using ASTNodeVariant = std::variant<{}>;

template <typename R>
class ASTVisitor : public Visitor<R, {}> {{

public:
  R operator()(ASTNode const *) override {{
    return nullptr;
  }}

}};

}} // namespace claire::parser
"""

if __name__ == "__main__":
    forward_decls = ""
    variant_types = []

    for node_type in AST_NODE_TYPES:
        forward_decls += f"class {node_type};\n"
        variant_types.append(f"{node_type} const *")

    variant_types = ",".join(variant_types)
    template_types = ",".join(AST_NODE_TYPES)

    print(CODE_TEMPLATE.format(forward_decls, variant_types, template_types))
