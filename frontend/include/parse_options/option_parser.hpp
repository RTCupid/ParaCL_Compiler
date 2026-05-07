#ifndef INCLUDE_OPTION_PARSER_HPP
#define INCLUDE_OPTION_PARSER_HPP

#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>

namespace language {
    
struct Options {
    std::string output_name_;
    llvm::OptimizationLevel optimization_level_;
};

class Option_parser final {
private:
    const int argc_;
    const char** argv_;

    Options options;

public:
    Option_parser(const int argc, const char** argv) : argc_(argc), argv_(argv) {} 
    Options parse_options();

private:
    llvm::OptimizationLevel parse_optimization_level(const char* parse_string);
    std::string parse_output_name(const char* parse_string);
};

} // namespace language

#endif // INCLUDE_OPTION_PARSER_HPP