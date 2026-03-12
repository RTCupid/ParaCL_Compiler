#ifndef FRONTEND_INCLUDE_CODEGEN_HPP
#define FRONTEND_INCLUDE_CODEGEN_HPP

#include "codegen/scope_stack.hpp"
#include "data_structures/node.hpp"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/IR/Instructions.h>
#include <llvm/TargetParser/Host.h>
#include <iostream>

namespace language {

class Code_generator final : public ASTVisitor {
  private:
    llvm::LLVMContext context_;
    llvm::Module module_;
    llvm::IRBuilder<> builder_;

    Scope_stack scope_stack_;

    llvm::Function *current_function_ = nullptr;
    functions_table_t functions_;

    llvm::Value *last_value_;

  public:
    Code_generator(const std::string &module_name);
    void print() const;
    void compile(const std::string& ir_file, const std::string& exe_file);

    void visit(Program &node) override;
    void visit(Block_stmt &node) override;

    void visit(Empty_stmt &node) override;

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
    llvm::FunctionCallee get_func(const std::string &name);
    llvm::FunctionCallee get_printf();
    llvm::FunctionCallee get_scanf();
};

} // namespace language

#endif // FRONTEND_INCLUDE_CODEGEN_HPP
