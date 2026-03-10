#ifndef FRONTEND_INCLUDE_SCOPE_STACK_HPP
#define FRONTEND_INCLUDE_SCOPE_STACK_HPP

#include "config.hpp"
#include <llvm-18/llvm/IR/Instructions.h>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace language {

class Scope_stack final {
  private:
    std::vector<symbol_table_t> scopes_;

  public:
    Scope_stack() {
        push(); // add global scope
    }

    void push(symbol_table_t nametable = {}) {
        scopes_.emplace_back(std::move(nametable));
    }

    void pop() {
        if (scopes_.empty()) {
            throw std::underflow_error("pop() called with empty scope stack");
        }
        scopes_.pop_back();
    }

    llvm::AllocaInst *lookup(name_t_sv var_name) const {
        if (scopes_.empty())
            return {};

        const std::string key(var_name);

        for (const auto &scope : scopes_ | std::views::reverse) {
            if (const auto &f = scope.find(key); f != scope.end()) {
                return f->second;
            }
        }

        return nullptr;
    }

    bool find(name_t_sv var_name) const { return !lookup(var_name); }

    void declare(name_t_sv var_name, llvm::AllocaInst *alloca) {
        if (scopes_.empty()) {
            throw std::underflow_error(
                "add_variable() called with empty scope stack");
        }

        std::string key(var_name);

        if (find(key)) {
            throw std::runtime_error("variable already declared in this scope");
        }

        scopes_.back()[key] = alloca;
    }
};

} // namespace language

#endif // FRONTEND_INCLUDE_SCOPE_STACK_HPP
