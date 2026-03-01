#ifndef FRONTEND_INCLUDE_SCOPE_HPP
#define FRONTEND_INCLUDE_SCOPE_HPP

#include "config.hpp"
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace language {

class Scope final {
  private:
    std::vector<nametable_t> scopes_;
    std::vector<nametable_t> archived_;

  public:
    Scope() {
        push(); // add global scope
    }

    void push(nametable_t nametable = {}) {
        scopes_.emplace_back(std::move(nametable));
    }

    void pop() {
        if (scopes_.empty()) {
            throw std::underflow_error("pop() called with empty scope stack");
        }
        archived_.push_back(std::move(scopes_.back()));
        scopes_.pop_back();
    }

    name_t_sv lookup(name_t_sv var_name) const {
        if (scopes_.empty())
            return {};

        const std::string key(var_name);

        for (const auto &scope : scopes_ | std::views::reverse) {
            if (const auto &f = scope.find(key); f != scope.end()) {
                return std::string_view(*f);
            }
        }
        return {};
    }

    bool find(name_t_sv var_name) const { return !lookup(var_name).empty(); }

    name_t_sv add_variable(name_t_sv var_name) {
        if (scopes_.empty()) {
            throw std::underflow_error(
                "add_variable() called with empty scope stack");
        }

        if (auto existing = lookup(var_name); !existing.empty()) {
            return existing;
        }

        auto [it, inserted] = scopes_.back().emplace(std::string(var_name));
        return std::string_view(*it);
    }
};

} // namespace language

#endif // FRONTEND_INCLUDE_UTILS_HPP
