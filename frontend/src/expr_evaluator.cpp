#include "simulator/expr_evaluator.hpp"
#include "simulator/simulator.hpp"
#include "simulator/helpers.hpp"
#include "scope.hpp"
#include <iostream>
#include <string>

namespace language {

value_t Expression_evaluator::get_result() const noexcept { return result_; }

void Expression_evaluator::visit(Number &node) { result_ = node.get_value(); }

void Expression_evaluator::visit(Variable &node) {
    auto var_name = std::string{node.get_name()};

    // auto it = nametable.find(var_name);

    auto res = simulator_.get_scopes().lookup(var_name);
    if (res.empty()) {
        throw std::runtime_error("Unknown variable: " + var_name);       
    }
    result_ = res;
}

void Expression_evaluator::visit(Assignment_expr &node) {
    auto var_name = std::string{node.get_variable()->get_name()};

    Expression_evaluator result_eval{simulator_};
    node.get_value().accept(result_eval);
    result_ = result_eval.result_;

    // auto &&it = nametable.find(var_name);
    auto res = simulator_.get_scopes().
    if (it != nametable.end())
        it->second = result_;
    else
        nametable.emplace(var_name, result_);
};

void Expression_evaluator::visit(Binary_operator &node) {
    Expression_evaluator left_eval{simulator_};
    node.get_left().accept(left_eval);
    auto left_num = expect_number(left_eval.result_, "Binary operator");

    Expression_evaluator right_eval{simulator_};
    node.get_right().accept(right_eval);
    auto right_num = expect_number(right_eval.result_, "Binary operator");

    switch (node.get_operator()) {
    case Binary_operators::Eq: {
        result_ = (left_num == right_num);
        break;
    }
    case Binary_operators::Neq: {
        result_ = (left_num != right_num);
        break;
    }
    case Binary_operators::Less: {
        result_ = (left_num < right_num);
        break;
    }
    case Binary_operators::LessEq: {
        result_ = (left_num <= right_num);
        break;
    }
    case Binary_operators::Greater: {
        result_ = (left_num > right_num);
        break;
    }
    case Binary_operators::GreaterEq: {
        result_ = (left_num >= right_num);
        break;
    }
    case Binary_operators::Add: {
        result_ = left_num + right_num;
        break;
    }
    case Binary_operators::Sub: {
        result_ = left_num - right_num;
        break;
    }
    case Binary_operators::Mul: {
        result_ = left_num * right_num;
        break;
    }
    case Binary_operators::Div: {
        result_ = left_num / right_num;
        break;
    }
    case Binary_operators::RemDiv: {
        result_ = left_num % right_num;
        break;
    }
    case Binary_operators::And: {
        result_ = left_num & right_num;
        break;
    }
    case Binary_operators::Xor: {
        result_ = left_num ^ right_num;
        break;
    }
    case Binary_operators::Or: {
        result_ = left_num | right_num;
        break;
    }
    case Binary_operators::LogOr: {
        result_ = left_num || right_num;
        break;
    }
    case Binary_operators::LogAnd: {
        result_ = left_num && right_num;
        break;
    }
    default:
        throw std::runtime_error("Unknown binary operator");
    }
}

void Expression_evaluator::visit(Unary_operator &node) {
    Expression_evaluator eval{simulator_};
    node.get_operand().accept(eval);
    auto num = expect_number(eval.result_, "Unary operator");

    switch (node.get_operator()) {
    case Unary_operators::Neg: {
        result_ = -(num);
        break;
    }
    case Unary_operators::Plus: {
        result_ = num;
        break;
    }
    case Unary_operators::Not: {
        result_ = !num;
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
void Expression_evaluator::visit(If_stmt &node) {}
void Expression_evaluator::visit(While_stmt &node) {}
void Expression_evaluator::visit(Print_stmt &node) {}

void Expression_evaluator::visit(Call &node) {
    Expression_evaluator target_eval{simulator_};
    node.get_target().accept(target_eval);
    auto target_result = target_eval.result_;
    auto fn = expect_function(target_result, "Call");

    if (fn->get_params().size() != node.get_args().size()) {
        throw std::runtime_error("Too few arguments in function call");
    }

    std::vector<value_t> arg_values;
    arg_values.reserve(node.get_args().size());
    for (auto&& arg : node.get_args()) {
        Expression_evaluator arg_eval{simulator_};
        arg->accept(arg_eval);
        arg_values.push_back(arg_eval.get_result());
    }

    for (size_t i = 0; i != fn->get_params().size(); ++i) {
        const auto& param_name = std::string(fn->get_params()[i]);
        simulator_.get_nametable()[param_name] = arg_values[i];
        // std::cerr << param_name << " = " << std::get<number_t>(arg_values[i]) << "\n";
    }

    fn->get_body().accept(simulator_);

    if (simulator_.has_return_value()) {
        result_ = simulator_.take_return_value();
        // std::cerr << "res = " << std::get<number_t>(result_);
    } else {
        result_ = number_t{};
    }
}
void Expression_evaluator::visit(Func &node) {
    result_ = &node;
}
void Expression_evaluator::visit(Return_stmt &) {}
void Expression_evaluator::visit(Expr_stmt &) {}

} // namespace language
