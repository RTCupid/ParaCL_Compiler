#include "codegen/codegen.hpp"
#include "node.hpp"
#include <iostream>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <string>

namespace language {

Code_generator::Code_generator(const std::string &module_name)
    : module_{module_name, context_}, builder_{context_} {
    module_.setTargetTriple(llvm::sys::getProcessTriple());
}

void Code_generator::print() const {
    module_.print(llvm::outs(), nullptr);
}

void Code_generator::compile(const std::string& ir_file, const std::string& exe_file) {
    std::error_code EC;
    llvm::raw_fd_ostream OS(ir_file, EC);
    module_.print(OS, nullptr);

    std::string cmd = "clang " + ir_file + " -o " + exe_file;
    int result = std::system(cmd.c_str());
}

void Code_generator::visit(Program &node) {
    auto *main_ty =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(context_), false);

    auto *main_func =
        llvm::Function::Create(main_ty,
                               llvm::Function::ExternalLinkage,
                               "main",
                               module_);

    auto *entry_bb = llvm::BasicBlock::Create(context_, "entry", main_func);

    builder_.SetInsertPoint(entry_bb);
    current_function_ = main_func;

    const auto &statements = node.get_stmts();
    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }

    if (!builder_.GetInsertBlock()->getTerminator()) {
        llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0);
        builder_.CreateRet(zero);
    }
}

void Code_generator::visit(Block_stmt &node) {
    const auto &statements = node.get_stmts();

    for (const auto &stmt : statements) {
        stmt->accept(*this);
    }
}

void Code_generator::visit(Assignment_expr &node) {
    auto var_name = std::string{node.get_variable()->get_name()};

    node.get_value().accept(*this);
    auto value = last_value_;

    auto alloca = scope_stack_.lookup(var_name);

    if (!alloca) {
        llvm::Function *func = builder_.GetInsertBlock()->getParent();
        llvm::IRBuilder<> tmpBuilder(&func->getEntryBlock(),
                                     func->getEntryBlock().begin());
        alloca = tmpBuilder.CreateAlloca(llvm::Type::getInt32Ty(context_),
                                         nullptr, var_name);

        scope_stack_.declare(var_name, alloca);
    }

    builder_.CreateStore(value, alloca);
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
    node.get_operand().accept(*this);
    auto value = last_value_;
    switch (node.get_operator()) {
    case Unary_operators::Neg: {
        last_value_ = builder_.CreateNeg(value, "neg");
        break;
    }
    case Unary_operators::Plus: {
        last_value_ = value;
        break;
    }
    case Unary_operators::Not: {
        auto not_bool = builder_.CreateICmpEQ(value, 0, "eqtmp");
        last_value_ = builder_.CreateZExt(
            not_bool, llvm::Type::getInt32Ty(context_), "unarynotzext");
        break;
    }
    default:
        throw std::runtime_error("Unknown unary operator");
    }
}

void Code_generator::visit(Input &node) {
    std::string var_name = "__input_var";
    llvm::AllocaInst *alloca = scope_stack_.lookup(var_name);

    if (!alloca) {
        llvm::Function *func = builder_.GetInsertBlock()->getParent();
        llvm::IRBuilder<> tmpBuilder(&func->getEntryBlock(),
                                     func->getEntryBlock().begin());
        alloca = tmpBuilder.CreateAlloca(llvm::Type::getInt32Ty(context_),
                                         nullptr, var_name);

        scope_stack_.declare(var_name, alloca);
    }

    llvm::Value *format_str = builder_.CreateGlobalStringPtr("%d", "scanf_fmt");
    llvm::FunctionCallee scanf_func = get_scanf();

    builder_.CreateCall(
        scanf_func,
        {format_str, alloca},
        "scanfcall"
    );

    last_value_ = builder_.CreateLoad(
        alloca->getAllocatedType(),
        alloca,
        var_name
    );
}

void Code_generator::visit(Print_stmt &node) {
    node.get_value().accept(*this);

    llvm::Value *value_to_print = last_value_;

    llvm::Value *format_str = builder_.CreateGlobalStringPtr("%d\n", "print_fmt");
    llvm::FunctionCallee printf_func = get_printf();

    last_value_ = builder_.CreateCall(
        printf_func,
        {format_str, value_to_print},
        "printfcall"
    );
}

void Code_generator::visit(If_stmt &node) {
    node.get_condition().accept(*this);

    llvm::Value *cond = builder_.CreateICmpNE(last_value_,
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0), "ifcond");

    auto *then_bb = llvm::BasicBlock::Create(context_, "then", current_function_);

    bool contains_else = node.contains_else_branch();

    llvm::BasicBlock* else_bb = nullptr;

    if (contains_else)
        else_bb = llvm::BasicBlock::Create(context_, "else", current_function_);

    auto merge_bb = llvm::BasicBlock::Create(context_, "ifcont", current_function_);

    if (contains_else)
        last_value_ = builder_.CreateCondBr(cond, then_bb, else_bb);
    else
        last_value_ = builder_.CreateCondBr(cond, then_bb, merge_bb);

    builder_.SetInsertPoint(then_bb);
    node.then_branch().accept(*this);
    if (!builder_.GetInsertBlock()->getTerminator())
        builder_.CreateBr(merge_bb);

    if (contains_else) {
        builder_.SetInsertPoint(else_bb);
        node.else_branch().accept(*this);
        if (!builder_.GetInsertBlock()->getTerminator())
            builder_.CreateBr(merge_bb);
    }

    builder_.SetInsertPoint(merge_bb);
}

void Code_generator::visit(While_stmt &node) {
    auto *cond_bb = llvm::BasicBlock::Create(context_, "cond_loop", current_function_);

    last_value_ = builder_.CreateBr(cond_bb);

    builder_.SetInsertPoint(cond_bb);

    node.get_condition().accept(*this);

    llvm::Value *cond = builder_.CreateICmpNE(last_value_, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0), "ifcond");

    auto *body_bb = llvm::BasicBlock::Create(context_, "body_loop", current_function_);
    auto *end_loop_bb = llvm::BasicBlock::Create(context_, "end_loop", current_function_);

    last_value_ = builder_.CreateCondBr(cond, body_bb, end_loop_bb);

    builder_.SetInsertPoint(body_bb);
    node.get_body().accept(*this);

    last_value_ = builder_.CreateBr(cond_bb);

    builder_.SetInsertPoint(end_loop_bb);
}

void Code_generator::visit(Func &node) {
    std::vector<llvm::Type *> param_types;
    const auto &params = node.get_params();

    for (size_t i = 0, params_size = params.size(); i < params_size; ++i) {
        param_types.push_back(llvm::Type::getInt32Ty(context_));
    }

    llvm::Type *return_type = llvm::Type::getInt32Ty(context_);
    llvm::FunctionType *func_type = llvm::FunctionType::get(
        return_type,
        param_types,
        /*isVarArg=*/false
    );

    std::string func_name;
    if (node.has_name()) {
        func_name = std::string(node.get_func_name().value());
    } else {
        func_name = "func_" + std::to_string(func_counter_++);
    }
    llvm::Function *llvm_func = llvm::Function::Create(
        func_type,
        llvm::Function::ExternalLinkage,
        func_name,
        module_
    );

    functions_[func_name] = llvm_func;
    llvm::Function *saved_function = current_function_;
}

void Code_generator::visit(Call &node) {}

void Code_generator::visit(Return_stmt &node) {
    if (node.has_value()) {
        node.get_value().accept(*this);
        llvm::Value *ret_val = last_value_;

        builder_.CreateRet(ret_val);
    } else {
        llvm::Value *zero = llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(context_),
            0
        );
        builder_.CreateRet(zero);
    }
}

void Code_generator::visit(Expr_stmt &node) {
    node.get_expr().accept(*this);
}

void Code_generator::visit(Number &node) {
    last_value_ = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_),
                                         node.get_value());
}

void Code_generator::visit(Variable &node) {
    auto var_name = std::string(node.get_name());
    llvm::AllocaInst *alloca = scope_stack_.lookup(var_name);

    if (!alloca)
        throw std::runtime_error("use of undeclared variable: " + var_name);

    last_value_ =
        builder_.CreateLoad(alloca->getAllocatedType(), alloca, var_name);
}

void Code_generator::visit(Empty_stmt &node) {
    // nothing needs to be done
};

llvm::FunctionCallee Code_generator::get_func(const std::string &name) {
    auto type = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context_),
        llvm::PointerType::get(llvm::Type::getInt8Ty(context_), 0), true);

    return module_.getOrInsertFunction(name, type);
}

llvm::FunctionCallee Code_generator::get_printf() {
    return get_func("printf");
}

llvm::FunctionCallee Code_generator::get_scanf() {
    return get_func("scanf");
}

} // namespace language
