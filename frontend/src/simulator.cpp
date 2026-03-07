#include "simulator.hpp"
#include "expr_evaluator.hpp"
#include "node.hpp"
#include <iostream>
#include <unordered_map>

namespace language {

void Simulator::visit(Program &node) {
    const auto &statements = node.get_stmts();

    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }
}

void Simulator::visit(Block_stmt &node) {
    const auto &statements = node.get_stmts();

    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }
}

void Simulator::visit(Empty_stmt &node) {};

void Simulator::visit(Assignment_stmt &node) {
    auto var_name = static_cast<std::string>(node.get_variable()->get_name());
    const auto &value = evaluate_expression(node.get_value());

    auto it = nametable_.find(var_name);
    if (it != nametable_.end())
        it->second = value;
    else
        nametable_.emplace(var_name, value);
}

void Simulator::visit(If_stmt &node) {
    auto condition = evaluate_expression(node.get_condition());

    if (condition != 0) {
        node.then_branch().accept(*this);
    } else {
        const bool contains_else_node = node.contains_else_branch();

        if (contains_else_node)
            node.else_branch().accept(*this);
    }
}

void Simulator::visit(While_stmt &node) {
    while (evaluate_expression(node.get_condition())) {
        node.get_body().accept(*this);
    }
}

void Simulator::visit(Print_stmt &node) {
    auto value = evaluate_expression(node.get_value());

    std::cout << value << '\n';
}

void Simulator::visit(Assignment_expr &node) {}
void Simulator::visit(Binary_operator &node) {}
void Simulator::visit(Input &node) {}
void Simulator::visit(Unary_operator &node) {}
void Simulator::visit(Number &node) {}
void Simulator::visit(Variable &node) {}

void Simulator::visit(Func &node) {}
void Simulator::visit(Call &node) {}
void Simulator::visit(Return_stmt &node) {}

void Simulator::visit(Expr_stmt &node) { evaluate_expression(node.get_expr()); }

number_t Simulator::evaluate_expression(Expression &expression) {
    Expression_evaluator evaluator(*this);
    expression.accept(evaluator);
    return evaluator.get_result();
}

} // namespace language
