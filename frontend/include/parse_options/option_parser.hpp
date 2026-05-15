#ifndef INCLUDE_OPTION_PARSER_HPP
#define INCLUDE_OPTION_PARSER_HPP

#include <llvm/Passes/OptimizationLevel.h>
#include <optional>
#include <string>

namespace language {

struct Options {
    std::string input_file_;
    std::optional<std::string> output_name_;
    llvm::OptimizationLevel optimization_level_ = llvm::OptimizationLevel::O2;
};

class Option_parser final {
  private:
    const int argc_;
    const char **argv_;

    Options options_;

  public:
    Option_parser(const int argc, const char **argv)
        : argc_(argc), argv_(argv) {}
    Options parse_options();

  private:
    llvm::OptimizationLevel parse_optimization_level(const char *parse_string);
    std::string parse_output_name(const char *parse_string);
};

} // namespace language

#endif // INCLUDE_OPTION_PARSER_HPP