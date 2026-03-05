#include "expr_evaluator.hpp"
#include "simulator.hpp"
#include <iostream>
#include <string>

namespace language {

number_t Expression_evaluator::get_result() const noexcept { return result_; }

void Expression_evaluator::visit(Number &node) { result_ = node.get_value(); }

void Expression_evaluator::visit(Variable &node) {
    auto &nametable = simulator_.get_nametable();
    auto var_name = std::string{node.get_name()};

    auto it = nametable.find(var_name);
    if (it != nametable.end()) {
        result_ = it->second;
    } else {
        throw std::runtime_error("Unknown variable: " + var_name);
    }
}

void Expression_evaluator::visit(Assignment_expr &node) {
    auto &nametable = simulator_.get_nametable();
    auto var_name = std::string{node.get_variable()->get_name()};

    Expression_evaluator result_eval{simulator_};
    node.get_value().accept(result_eval);
    result_ = result_eval.result_;

    auto &&it = nametable.find(var_name);
    if (it != nametable.end())
        it->second = result_;
    else
        nametable.emplace(var_name, result_);
};

void Expression_evaluator::visit(Binary_operator &node) {
    Expression_evaluator left_eval{simulator_};
    node.get_left().accept(left_eval);
    auto left_value = left_eval.result_;

    Expression_evaluator right_eval{simulator_};
    node.get_right().accept(right_eval);
    auto right_value = right_eval.result_;

    switch (node.get_operator()) {
    case Binary_operators::Eq: {
        result_ = (left_value == right_value);
        break;
    }
    case Binary_operators::Neq: {
        result_ = (left_value != right_value);
        break;
    }
    case Binary_operators::Less: {
        result_ = (left_value < right_value);
        break;
    }
    case Binary_operators::LessEq: {
        result_ = (left_value <= right_value);
        break;
    }
    case Binary_operators::Greater: {
        result_ = (left_value > right_value);
        break;
    }
    case Binary_operators::GreaterEq: {
        result_ = (left_value >= right_value);
        break;
    }
    case Binary_operators::Add: {
        result_ = left_value + right_value;
        break;
    }
    case Binary_operators::Sub: {
        result_ = left_value - right_value;
        break;
    }
    case Binary_operators::Mul: {
        result_ = left_value * right_value;
        break;
    }
    case Binary_operators::Div: {
        result_ = left_value / right_value;
        break;
    }
    case Binary_operators::RemDiv: {
        result_ = left_value % right_value;
        break;
    }
    case Binary_operators::And: {
        result_ = left_value & right_value;
        break;
    }
    case Binary_operators::Xor: {
        result_ = left_value ^ right_value;
        break;
    }
    case Binary_operators::Or: {
        result_ = left_value | right_value;
        break;
    }
    case Binary_operators::LogOr: {
        result_ = left_value || right_value;
        break;
    }
    case Binary_operators::LogAnd: {
        result_ = left_value && right_value;
        break;
    }
    default:
        throw std::runtime_error("Unknown binary operator");
    }
}

void Expression_evaluator::visit(Unary_operator &node) {
    Expression_evaluator eval{simulator_};
    node.get_operand().accept(eval);
    auto value = eval.result_;
    switch (node.get_operator()) {
    case Unary_operators::Neg: {
        result_ = -(value);
        break;
    }
    case Unary_operators::Plus: {
        result_ = value;
        break;
    }
    case Unary_operators::Not: {
        result_ = !value;
        break;
    }
    default:
        throw std::runtime_error("Unknown unary operator");
    }
}

void Expression_evaluator::visit(Input &node) {
    number_t value;
    std::cin >> value;

    result_ = value;
}

void Expression_evaluator::visit(Program &node) {}
void Expression_evaluator::visit(Block_stmt &node) {}
void Expression_evaluator::visit(Empty_stmt &node) {}
void Expression_evaluator::visit(Assignment_stmt &node) {}
void Expression_evaluator::visit(If_stmt &node) {}
void Expression_evaluator::visit(While_stmt &node) {}
void Expression_evaluator::visit(Print_stmt &node) {}

void Expression_evaluator::visit(Call &) {}
void Expression_evaluator::visit(Func &) {}
} // namespace language
