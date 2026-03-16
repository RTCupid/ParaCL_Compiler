#include "simulator/simulator.hpp"
#include "node.hpp"
#include "simulator/expr_evaluator.hpp"
#include <iostream>

namespace language {

void Simulator::visit(Program &node) {
    const auto &statements = node.get_stmts();

    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }
}

void Simulator::visit(Block_expr &node) {
    const auto &statements = node.get_stmts();

    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }
}

void Simulator::visit(Empty_stmt &node) {};

void Simulator::visit(If_stmt &node) {
    auto condition = evaluate_expression(node.get_condition());

    if (condition != 0) {
        evaluate_expression(node.then_branch());
    } else if (node.contains_else_branch()) {
        evaluate_expression(node.else_branch());
    }
}

void Simulator::visit(While_stmt &node) {
    while (evaluate_expression(node.get_condition())) {
        evaluate_expression(node.get_body());
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
