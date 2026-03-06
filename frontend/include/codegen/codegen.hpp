#ifndef FRONTEND_INCLUDE_CODEGEN_HPP
#define FRONTEND_INCLUDE_CODEGEN_HPP

#include "data_structures/node.hpp"

#include "llvm/IR/Argument.h"   // Аргументы функции
#include "llvm/IR/BasicBlock.h" // Класс BasicBlock
#include "llvm/IR/DerivedTypes.h" // Содержит FunctionType, StructType, PointerType и др.
#include "llvm/IR/Function.h" // Класс Function
#include "llvm/IR/IRBuilder.h" // Построитель инструкций (IRBuilder<>)
#include "llvm/IR/LLVMContext.h" // Управление контекстом (LLVMContext)
#include "llvm/IR/Module.h" // Представляет весь модуль (Module)
#include "llvm/IR/Type.h" // Базовый класс Type
#include "llvm/IR/Value.h" // Базовый класс для всех значений (Value)
#include "llvm/Support/raw_ostream.h" // Для вывода (например, module.print(llvm::outs()))
#include <llvm-18/llvm/IR/Instructions.h>
#include <unordered_map>
#include <vector>

namespace language {

class Code_generator : public ASTVisitor {
  private:
    llvm::LLVMContext context_;
    llvm::Module module_;
    llvm::IRBuilder<> builder_;

    std::unordered_map<std::string, llvm::AllocaInst *> symbol_table_;
    std::vector<std::unordered_map<std::string, llvm::AllocaInst *>>
        scope_stack_;

    llvm::Function *current_function_ = nullptr;

    llvm::Value *last_value_;

  public:
    Code_generator(const std::string &module_name)
        : module_{module_name, context_}, builder_{context_} {
        module_.setTargetTriple("x86_64-unknown-unknown");
    }

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
};

} // namespace language

#endif // FRONTEND_INCLUDE_CODEGEN_HPP
