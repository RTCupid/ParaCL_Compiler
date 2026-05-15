#include "parse_options/option_parser.hpp"

#include <stdexcept>
#include <string>
#include <string_view>

namespace language {
namespace {

bool is_option(std::string_view value) {
    return value.starts_with('-') && value != "-";
}

const char *require_value(int &index, int argc, const char **argv,
                          std::string_view option_name) {
    if (index + 1 >= argc)
        throw std::runtime_error("Option " + std::string{option_name} +
                                 " requires an argument");
    return argv[++index];
}

} // namespace

Options Option_parser::parse_options() {
    for (int i = 1; i < argc_; ++i) {
        const std::string_view arg{argv_[i]};

        if (arg == "--") {
            for (++i; i < argc_; ++i) {
                if (!options_.input_file_.empty())
                    throw std::runtime_error("Multiple input files not supported");
                options_.input_file_ = argv_[i];
            }
            break;
        }

        if (arg == "-o" || arg == "--output") {
            options_.output_name_ =
                parse_output_name(require_value(i, argc_, argv_, arg));
            continue;
        }

        if (arg.starts_with("--output=")) {
            options_.output_name_ =
                parse_output_name(argv_[i] + std::string_view{"--output="}.size());
            continue;
        }

        if (arg.starts_with("-o") && arg.size() > 2) {
            options_.output_name_ = parse_output_name(argv_[i] + 2);
            continue;
        }

        if (arg == "-O") {
            options_.optimization_level_ =
                parse_optimization_level(require_value(i, argc_, argv_, arg));
            continue;
        }

        if (arg.starts_with("-O") && arg.size() > 2) {
            options_.optimization_level_ = parse_optimization_level(argv_[i] + 2);
            continue;
        }

        if (is_option(arg))
            throw std::runtime_error("Unknown option: " + std::string{arg});

        if (!options_.input_file_.empty())
            throw std::runtime_error("Multiple input files not supported");
        options_.input_file_ = arg;
    }

    if (options_.input_file_.empty())
        throw std::runtime_error("No input file provided");

    return options_;
}

llvm::OptimizationLevel
Option_parser::parse_optimization_level(const char *parse_string) {
    const std::string_view level{parse_string};

    if (level == "0")
        return llvm::OptimizationLevel::O0;
    if (level == "1")
        return llvm::OptimizationLevel::O1;
    if (level == "2")
        return llvm::OptimizationLevel::O2;
    if (level == "3")
        return llvm::OptimizationLevel::O3;
    if (level == "s")
        return llvm::OptimizationLevel::Os;
    if (level == "z")
        return llvm::OptimizationLevel::Oz;

    throw std::runtime_error("Unknown optimization level: -O" +
                             std::string{level});
}

std::string Option_parser::parse_output_name(const char *parse_string) {
    if (std::string_view{parse_string}.empty())
        throw std::runtime_error("Output file name cannot be empty");
    return parse_string;
}

} // namespace language
