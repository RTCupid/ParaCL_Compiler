#include "simulator/simulator.hpp"
#include "node.hpp"
#include "simulator/expr_evaluator.hpp"
#include "simulator/helpers.hpp"
#include <iostream>

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

void Simulator::visit(If_stmt &node) {
    auto condition = expect_number(evaluate_expression(node.get_condition()), "If");

    if (condition) {
        node.then_branch().accept(*this);
    } else {
        const bool contains_else_node = node.contains_else_branch();

        if (contains_else_node)
            node.else_branch().accept(*this);
    }
}

void Simulator::visit(While_stmt &node) {
    while (expect_number(evaluate_expression(node.get_condition()), "While")) {
        node.get_body().accept(*this);
    }
}

void Simulator::visit(Print_stmt &node) {
    auto number = expect_number(evaluate_expression(node.get_value()), "Print");
    std::cout << number << '\n';
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

value_t Simulator::evaluate_expression(Expression &expression) {
    Expression_evaluator evaluator(*this);
    expression.accept(evaluator);
    return evaluator.get_result();
}

} // namespace language
