#pragma once

#include "config.hpp"
#include <variant>

namespace language {

inline number_t expect_number(const value_t& value, const std::string& s) {
    if (!std::holds_alternative<number_t>(value)) {
        throw std::runtime_error(s + " expects number");
    }
    return std::get<number_t>(value);
} 

inline function_t expect_function(const value_t& value, const std::string& s) {
    if (!std::holds_alternative<function_t>(value)) {
        throw std::runtime_error(s + " expects function");
    }
    return std::get<function_t>(value);
}

} // namespace language
