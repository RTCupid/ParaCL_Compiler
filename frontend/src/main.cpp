#include "driver.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>

extern int yylex();
yy::parser::semantic_type *yylval = nullptr;

int yyFlexLexer::yywrap() { return 1; }

int main(int argc, const char *argv[]) {
    try {
        driver(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "unknown error\n";
        return 2;
    }
}
