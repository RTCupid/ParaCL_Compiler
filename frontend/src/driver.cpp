#include "driver.hpp"
#include "codegen/codegen.hpp"
#include "dump_path_gen.hpp"
#include "compile_path_gen.hpp"
#include "graph_dump.hpp"
#include "lexer.hpp"
#include "my_parser.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "simulator/simulator.hpp"
#include <iostream>

void driver(int argc, const char **argv) {
    if (argc < 2) {
        throw std::runtime_error(std::string("Usage: ") + argv[0] +
                                 " <program_file>");
    }

    std::ifstream program_file(argv[1]);
    if (!program_file) {
        throw std::runtime_error("Cannot open program file\n");
    }
    language::Lexer scanner(&program_file, &std::cout);

    language::My_parser parser(&scanner, argv[1]);

    int result = parser.parse();

    language::program_ptr root = parser.get_root();

    if (parser.error_collector.has_errors()) {
        std::cout << "FAILED: ";
        parser.error_collector.print_errors(std::cout);
        throw std::runtime_error("parse failed\n");
    }

    if (result != 0) {
        throw std::runtime_error("unknown error\n");
    }

#ifdef GRAPH_DUMP
    // ____________GRAPH DUMP___________ //
    const auto paths = language::make_dump_paths();
    const std::string gv_file = paths.gv.string();
    const std::string svg_file = paths.svg.string();
    // dot dump/dump.gv -Tsvg -o dump/dump.svg

    std::ofstream gv(gv_file);
    if (!gv) {
        throw std::runtime_error("unable to open gv file\n");
    }
    language::graph_dump(gv, *root);
#endif

#ifdef INTERPRET
    language::Simulator simulator{};
    root->accept(simulator);
#else    
    const auto paths = language::make_compile_paths();
    const std::string ir_file = paths.ll.string();
    const std::string exe_file = paths.exe.string();

    const std::string& module_name = argv[1];
    language::Code_generator generator{module_name};
    root->accept(generator);

    generator.print(); // debug output generated LLVM IR

    generator.compile(ir_file, exe_file);
#endif
}
