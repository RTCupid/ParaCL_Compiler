#ifndef INCLUDE_GRAPH_DUMP
#define INCLUDE_GRAPH_DUMP

#include "node.hpp"
#include <fstream>

namespace language {

class Graph_dump final : public ASTVisitor {
  private:
    std::ostream &gv_;
    const Node *parent_;

  public:
    Graph_dump(std::ostream &gv, const Node *parent)
        : gv_(gv), parent_(parent) {}

    void visit(Program &node) override;
    void visit(Block_stmt &node) override;
    void visit(Empty_stmt &node) override;
    void visit(Assignment_stmt &node) override;
    void visit(Input &node) override;
    void visit(If_stmt &node) override;
    void visit(While_stmt &node) override;
    void visit(Print_stmt &node) override;
    void visit(Assignment_expr &node) override;
    void visit(Binary_operator &node) override;
    void visit(Unary_operator &node) override;
    void visit(Number &node) override;
    void visit(Variable &node) override;

    void visit(Func &node) override;
    void visit(Call &node) override;
    void visit(Return_stmt &node) override;
    void visit(Expr_stmt &node) override;

  private:
    void emit_edge(const Node *from, const Node *to) {
        gv_ << "    node_" << from << " -> node_" << to << ";\n";
    }
};

inline void graph_dump(std::ostream &gv, Node &root) {
    gv << "digraph G {\n"
       << "    rankdir=TB;\n"
       << "    node [style=filled, fontname=\"Helvetica\", fontcolor=darkblue, "
       << "fillcolor=peachpuff, color=\"#252A34\", penwidth=2.5];\n"
       << "    bgcolor=\"lemonchiffon\";\n\n";

    Graph_dump visitor{gv, nullptr};
    root.accept(visitor);

    gv << "\n}\n";
}

} // namespace language

#endif // INCLUDE_GRAPH_DUMP