#include "codegen/codegen.hpp"
#include "node.hpp"
#include <iostream>

namespace language {

void Code_generator::visit(Program &node) {
    const auto &statements = node.get_stmts();

    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }
}

void Code_generator::visit(Block_stmt &node) {
    const auto &statements = node.get_stmts();

    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }
}

void Code_generator::visit(Empty_stmt &node) {};

void Code_generator::visit(Assignment_stmt &node) {
    auto var_name = static_cast<std::string>(node.get_variable()->get_name());
    const auto &value = evaluate_expression(node.get_value());

    auto it = symbol_table_.find(var_name);
    if (it != symbol_table_.end())
        it->second = value;
    else
        symbol_table_.emplace(var_name, value);
}

void Code_generator::visit(If_stmt &node) {
    auto condition = evaluate_expression(node.get_condition());

    if (condition != 0) {
        node.then_branch().accept(*this);
    } else {
        const bool contains_else_node = node.contains_else_branch();

        if (contains_else_node)
            node.else_branch().accept(*this);
    }
}

void Code_generator::visit(While_stmt &node) {
    while (evaluate_expression(node.get_condition())) {
        node.get_body().accept(*this);
    }
}

void Code_generator::visit(Print_stmt &node) {}

void Code_generator::visit(Variable &node) {}

void Code_generator::visit(Assignment_expr &node) {
    auto &nametable = simulator_.get_nametable();
    auto var_name = std::string{node.get_variable()->get_name()};

    Code_generator last_value_eval{simulator_};
    node.get_value().accept(last_value_eval);
    last_value_ = last_value_eval.last_value_;

    auto &&it = nametable.find(var_name);
    if (it != nametable.end())
        it->second = last_value_;
    else
        nametable.emplace(var_name, last_value_);
};

void Code_generator::visit(Binary_operator &node) {
    node.get_left().accept(*this);
    auto left_value = last_value_;

    node.get_right().accept(*this);
    auto right_value = last_value_;

    switch (node.get_operator()) {
    case Binary_operators::Eq: {
        auto cmp = builder_.CreateICmpEQ(left_value, right_value, "eqtmp");
        last_value_ = builder_.CreateZExt(cmp, llvm::Type::getInt32Ty(context_),
                                          "eqzext");
        break;
    }
    case Binary_operators::Neq: {
        auto cmp = builder_.CreateICmpNE(left_value, right_value, "neqtmp");
        last_value_ = builder_.CreateZExt(cmp, llvm::Type::getInt32Ty(context_),
                                          "neqzext");
        break;
    }
    case Binary_operators::Less: {
        auto cmp = builder_.CreateICmpSLT(left_value, right_value, "lttmp");
        last_value_ = builder_.CreateZExt(cmp, llvm::Type::getInt32Ty(context_),
                                          "ltzext");
        break;
    }
    case Binary_operators::LessEq: {
        auto cmp = builder_.CreateICmpSLE(left_value, right_value, "letmp");
        last_value_ = builder_.CreateZExt(cmp, llvm::Type::getInt32Ty(context_),
                                          "lezext");
        break;
    }
    case Binary_operators::Greater: {
        auto cmp = builder_.CreateICmpSGT(left_value, right_value, "gttmp");
        last_value_ = builder_.CreateZExt(cmp, llvm::Type::getInt32Ty(context_),
                                          "gtzext");
        break;
    }
    case Binary_operators::GreaterEq: {
        auto cmp = builder_.CreateICmpSGE(left_value, right_value, "getmp");
        last_value_ = builder_.CreateZExt(cmp, llvm::Type::getInt32Ty(context_),
                                          "gezext");
    }
    case Binary_operators::Add: {
        last_value_ = builder_.CreateAdd(left_value, right_value, "addtmp");
        break;
    }
    case Binary_operators::Sub: {
        last_value_ = builder_.CreateSub(left_value, right_value, "subtmp");
        break;
    }
    case Binary_operators::Mul: {
        last_value_ = builder_.CreateMul(left_value, right_value, "multmp");
        break;
    }
    case Binary_operators::Div: {
        last_value_ = builder_.CreateSDiv(left_value, right_value, "divtmp");
        break;
    }
    case Binary_operators::RemDiv: {
        last_value_ = builder_.CreateSRem(left_value, right_value, "remtmp");
        break;
    }
    case Binary_operators::And: {
        last_value_ = builder_.CreateAnd(left_value, right_value, "andtmp");
        break;
    }
    case Binary_operators::Xor: {
        last_value_ = builder_.CreateXor(left_value, right_value, "xortmp");
        break;
    }
    case Binary_operators::Or: {
        last_value_ = builder_.CreateOr(left_value, right_value, "ortmp");
        break;
    }
    case Binary_operators::LogOr: {
        auto left_bool = builder_.CreateICmpNE(
            left_value, llvm::ConstantInt::get(left_value->getType(), 0),
            "leftbool");
        auto right_bool = builder_.CreateICmpNE(
            right_value, llvm::ConstantInt::get(right_value->getType(), 0),
            "rightbool");
        auto or_bool = builder_.CreateOr(left_bool, right_bool, "logor");
        last_value_ = builder_.CreateZExt(
            or_bool, llvm::Type::getInt32Ty(context_), "logorzext");
        break;
    }
    case Binary_operators::LogAnd: {
        auto left_bool = builder_.CreateICmpNE(
            left_value, llvm::ConstantInt::get(left_value->getType(), 0),
            "leftbool");
        auto right_bool = builder_.CreateICmpNE(
            right_value, llvm::ConstantInt::get(right_value->getType(), 0),
            "rightbool");
        auto and_bool = builder_.CreateAnd(left_bool, right_bool, "logand");
        last_value_ = builder_.CreateZExt(
            and_bool, llvm::Type::getInt32Ty(context_), "logandzext");
        break;
    }
    default:
        throw std::runtime_error("Unknown binary operator");
    }
}

void Code_generator::visit(Unary_operator &node) {
    Code_generator eval{simulator_};
    node.get_operand().accept(eval);
    auto value = eval.last_value_;
    switch (node.get_operator()) {
    case Unary_operators::Neg: {
        last_value_ = -(value);
        break;
    }
    case Unary_operators::Plus: {
        last_value_ = value;
        break;
    }
    case Unary_operators::Not: {
        last_value_ = !value;
        break;
    }
    default:
        throw std::runtime_error("Unknown unary operator");
    }
}

void Code_generator::visit(Input &node) {
    number_t value;
    std::cin >> value;

    last_value_ = value;
}

void Code_generator::visit(Number &node) {
    last_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_),
                                         node.get_value());
}

void Code_generator::visit(Func &node) {}

void Code_generator::visit(Call &node) {}

} // namespace language
