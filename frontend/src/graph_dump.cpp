#include "graph_dump.hpp"
#include "node.hpp"
#include <ostream>

namespace language {

void Graph_dump::visit(Program &node) {
    const auto &stmts = node.get_stmts();
    const std::size_t size = stmts.size();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=salmon"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ Program"
        << " | addr: " << &node << " | parent: " << parent_ << "| { ";

    for (std::size_t i = 0; i < size; ++i) {
        gv_ << "stmt_" << i << ": " << stmts[i];
        if (i + 1 < size)
            gv_ << " | ";
    }
    gv_ << " } }\"" << "];\n";

    for (auto *stmt : stmts) {
        if (!stmt)
            continue;
        emit_edge(&node, stmt);
        Graph_dump child{gv_, &node};
        stmt->accept(child);
    }
}

void Graph_dump::visit(Block_stmt &node) {
    const auto &stmts = node.get_stmts();
    const std::size_t size = stmts.size();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=lightgoldenrod1"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ Block"
        << " | addr: " << &node << " | parent: " << parent_ << "| { ";

    for (std::size_t i = 0; i < size; ++i) {
        gv_ << "stmt_" << i << ": " << stmts[i];
        if (i + 1 < size)
            gv_ << " | ";
    }
    gv_ << " } }\"" << "];\n";

    for (auto *stmt : stmts) {
        if (!stmt)
            continue;
        emit_edge(&node, stmt);
        Graph_dump child{gv_, &node};
        stmt->accept(child);
    }
}

void Graph_dump::visit(Empty_stmt &node) {
    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=lavenderblush1"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ Empty"
        << " | addr: " << &node << " | parent: " << parent_ << "}\"" << "];\n";
}

void Graph_dump::visit(Assignment_stmt &node) {
    auto *var = node.get_variable();
    auto *val = &node.get_value();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=plum"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; "
        << "label=\"{ Assignment" << " | addr: " << &node
        << " | parent: " << parent_ << "| { left: " << var
        << " | right: " << val << " } }\"" << "];\n";

    if (var) {
        emit_edge(&node, var);
        Graph_dump child{gv_, &node};
        var->accept(child);
    }

    emit_edge(&node, val);
    Graph_dump child{gv_, &node};
    val->accept(child);
}

void Graph_dump::visit(Assignment_expr &node) {
    auto *var = node.get_variable();
    auto *val = &node.get_value();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=plum"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; "
        << "label=\"{ Assignment expr" << " | addr: " << &node
        << " | parent: " << parent_ << "| { left: " << var
        << " | right: " << val << " } }\"" << "];\n";

    if (var) {
        emit_edge(&node, var);
        Graph_dump child{gv_, &node};
        var->accept(child);
    }

    emit_edge(&node, val);
    Graph_dump child{gv_, &node};
    val->accept(child);
}

void Graph_dump::visit(While_stmt &node) {
    auto *cond = &node.get_condition();
    auto *body = &node.get_body();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=turquoise"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ While"
        << " | addr: " << &node << " | parent: " << parent_
        << "| { left: " << cond << " | right: " << body << " } }\"" << "];\n";

    emit_edge(&node, cond);
    {
        Graph_dump child{gv_, &node};
        cond->accept(child);
    }

    emit_edge(&node, body);
    {
        Graph_dump child{gv_, &node};
        body->accept(child);
    }
}

void Graph_dump::visit(If_stmt &node) {
    auto *cond = &node.get_condition();
    auto *then_b = &node.then_branch();
    Statement *else_b =
        node.contains_else_branch() ? &node.else_branch() : nullptr;

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=turquoise"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ If"
        << " | addr: " << &node << " | parent: " << parent_
        << "| { cond: " << cond << " | then: " << then_b
        << " | else: " << else_b << " } }\"" << "];\n";

    emit_edge(&node, cond);
    {
        Graph_dump child{gv_, &node};
        cond->accept(child);
    }

    emit_edge(&node, then_b);
    {
        Graph_dump child{gv_, &node};
        then_b->accept(child);
    }

    if (else_b) {
        emit_edge(&node, else_b);
        Graph_dump child{gv_, &node};
        else_b->accept(child);
    }
}

void Graph_dump::visit(Input &node) {
    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=lavenderblush1"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ Input"
        << " | addr: " << &node << " | parent: " << parent_ << "}\"" << "];\n";
}

void Graph_dump::visit(Print_stmt &node) {
    auto *val = &node.get_value();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=darkorange"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ Print"
        << " | addr: " << &node << " | parent: " << parent_
        << " | value: " << val << "}\"" << "];\n";

    emit_edge(&node, val);
    Graph_dump child{gv_, &node};
    val->accept(child);
}

void Graph_dump::visit(Binary_operator &node) {
    const char *op_str = "";
    switch (node.get_operator()) {
    case Binary_operators::Eq:
        op_str = "==";
        break;
    case Binary_operators::Neq:
        op_str = "!=";
        break;
    case Binary_operators::Less:
        op_str = "\\<";
        break;
    case Binary_operators::LessEq:
        op_str = "\\<=";
        break;
    case Binary_operators::Greater:
        op_str = "\\>";
        break;
    case Binary_operators::GreaterEq:
        op_str = "\\>=";
        break;
    case Binary_operators::Add:
        op_str = "+";
        break;
    case Binary_operators::Sub:
        op_str = "-";
        break;
    case Binary_operators::Mul:
        op_str = "*";
        break;
    case Binary_operators::Div:
        op_str = "/";
        break;
    case Binary_operators::RemDiv:
        op_str = "%";
        break;
    case Binary_operators::And:
        op_str = "&";
        break;
    case Binary_operators::Xor:
        op_str = "^";
        break;
    case Binary_operators::Or:
        op_str = "\\|";
        break;
    case Binary_operators::LogAnd:
        op_str = "\\&&";
        break;
    case Binary_operators::LogOr:
        op_str = "\\|\\|";
        break;
    }

    auto *l = &node.get_left();
    auto *r = &node.get_right();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=lightsteelblue1"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; "
        << "label=\"{ Binary operator" << " | addr: " << &node
        << " | parent: " << parent_ << " | operator: " << op_str
        << " | { left: " << l << " | right: " << r << " } }\"" << "];\n";

    emit_edge(&node, l);
    {
        Graph_dump child{gv_, &node};
        l->accept(child);
    }

    emit_edge(&node, r);
    {
        Graph_dump child{gv_, &node};
        r->accept(child);
    }
}

void Graph_dump::visit(Unary_operator &node) {
    const char *op_str = "";
    switch (node.get_operator()) {
    case Unary_operators::Neg:
        op_str = "-";
        break;
    case Unary_operators::Plus:
        op_str = "+";
        break;
    case Unary_operators::Not:
        op_str = "!";
        break;
    }

    auto *opnd = &node.get_operand();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=lightsteelblue1"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; "
        << "label=\"{ Unary operator" << " | addr: " << &node
        << " | parent: " << parent_ << " | operator: " << op_str
        << "| operand: " << opnd << " }\"" << "];\n";

    emit_edge(&node, opnd);
    Graph_dump child{gv_, &node};
    opnd->accept(child);
}

void Graph_dump::visit(Number &node) {
    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=palegreen"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ Number"
        << " | addr: " << &node << " | parent: " << parent_
        << " | value: " << node.get_value() << " }\"" << "];\n";
}

void Graph_dump::visit(Variable &node) {
    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=cornflowerblue"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; "
        << "label=\"{ Variable" << " | addr: " << &node
        << " | parent: " << parent_ << " | name: " << node.get_name() << " }\""
        << "];\n";
}

void Graph_dump::visit(Func &node) {
    auto *body = &node.get_body();

    const auto name_opt = node.get_func_name();
    const char *name_str = name_opt ? "named" : "anonymous";

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=khaki1"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ Func"
        << " | addr: " << &node << " | parent: " << parent_ << " | "
        << name_str;

    if (name_opt) {
        gv_ << " | name: " << *name_opt;
    }

    gv_ << " | params_count: " << node.get_params().size()
        << " | body: " << body << " }\"" << "];\n";

    emit_edge(&node, body);
    Graph_dump child{gv_, &node};
    body->accept(child);
}

void Graph_dump::visit(Call &node) {
    auto *t = &node.get_target();

    gv_ << "    node_" << &node
        << "[shape=Mrecord; style=filled; fillcolor=lightskyblue1"
        << "; color=\"#000000\"; fontcolor=\"#000000\"; " << "label=\"{ Call"
        << " | addr: " << &node << " | parent: " << parent_
        << " | target: " << t << " | argc: " << node.get_args().size() << " }\""
        << "];\n";

    emit_edge(&node, t);
    {
        Graph_dump child{gv_, &node};
        t->accept(child);
    }

    for (auto *a : node.get_args()) {
        if (!a)
            continue;
        emit_edge(&node, a);
        Graph_dump child{gv_, &node};
        a->accept(child);
    }
}

} // namespace language
