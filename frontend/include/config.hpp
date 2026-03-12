#ifndef FRONTEND_INCLUDE_CONFIG_HPP
#define FRONTEND_INCLUDE_CONFIG_HPP

#include <llvm-18/llvm/IR/Instructions.h>
#include <string>
#include <string_view>
#include <unordered_set>
#include <variant>

namespace language {

class Func;

using number_t = int;
using function_t = Func*;
using value_t = std::variant<number_t, function_t>;

using name_t_sv = std::string_view;
using name_t = std::string;

using nametable_t = std::unordered_set<name_t>;

using symbol_table_t = std::unordered_map<name_t, llvm::AllocaInst *>;

class Program;

using program_ptr = Program *;

} // namespace language

#endif // FRONTEND_INCLUDE_CONFIG_HPP
