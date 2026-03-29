#include "driver.hpp"
#include "codegen/codegen.hpp"
#include "compile_path_gen.hpp"
#include "dump_path_gen.hpp"
#include "graph_dump.hpp"
#include "lexer.hpp"
#include "my_parser.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "simulator/simulator.hpp"
#include <iostream>
#include <utility>

void driver(int argc, const char **argv) {
    auto [opt_output_name, input_file] = language::parse_commandline_arguments(argc, argv);

    std::ifstream program_file(input_file);
    if (!program_file)
        throw std::runtime_error("Can't open program file\n");

    language::Lexer scanner(&program_file, &std::cout);
    language::My_parser parser(&scanner, input_file);
    int result = parser.parse();
    language::program_ptr root = parser.get_root();

    if (parser.error_collector.has_errors()) {
        std::cout << "FAILED: ";
        parser.error_collector.print_errors(std::cout);
        throw std::runtime_error("parse failed\n");
    }
    if (result != 0)
        throw std::runtime_error("unknown error\n");

#ifdef GRAPH_DUMP
    // ____________GRAPH DUMP___________ //
    const auto paths = language::make_dump_paths();
    const std::string gv_file = paths.gv.string();
    const std::string svg_file = paths.svg.string();

    std::ofstream gv(gv_file);
    if (!gv)
        throw std::runtime_error("unable to open gv file\n");

    language::graph_dump(gv, *root);
#endif

#ifdef INTERPRET
    language::Simulator simulator{};
    root->accept(simulator);
#else
    const std::string module_name = input_file;
    language::Compile_paths compile_paths = language::make_compile_paths(opt_output_name);

    const std::string ir_file = compile_paths.ll.string();
    const std::string exe_file = compile_paths.exe.string();

    language::Code_generator generator{input_file};
    root->accept(generator);
    generator.compile(ir_file, exe_file);
#endif
}
