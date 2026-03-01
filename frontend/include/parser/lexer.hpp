#ifndef FRONTEND_INCLUDE_LEXER_HPP
#define FRONTEND_INCLUDE_LEXER_HPP

#include "parser.hpp"
#include <fstream>

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

namespace language {

class Lexer final : public yyFlexLexer {
  public:
    int yylineno = 1;
    int yycolumn = 1;

    Lexer(std::istream *in, std::ostream *out) : yyFlexLexer(in, out) {}

    int get_line() const noexcept { return yylineno; }

    int get_column() const noexcept { return yycolumn; }
    int get_yyleng() const noexcept { return yyleng; }

    int process_if() const noexcept { return yy::parser::token::TOK_IF; }
    int process_else() const noexcept { return yy::parser::token::TOK_ELSE; }
    int process_while() const noexcept { return yy::parser::token::TOK_WHILE; }
    int process_print() const noexcept { return yy::parser::token::TOK_PRINT; }
    int process_input() const noexcept { return yy::parser::token::TOK_INPUT; }
    int process_plus() const noexcept { return yy::parser::token::TOK_PLUS; }
    int process_minus() const noexcept { return yy::parser::token::TOK_MINUS; }
    int process_mul() const noexcept { return yy::parser::token::TOK_MUL; }
    int process_rem_div() const noexcept { return yy::parser::token::TOK_REM_DIV; }
    int process_div() const noexcept { return yy::parser::token::TOK_DIV; }
    int process_and() const noexcept { return yy::parser::token::TOK_AND; }
    int process_xor() const noexcept { return yy::parser::token::TOK_XOR; }
    int process_or() const noexcept { return yy::parser::token::TOK_OR; }
    int process_log_or() const noexcept { return yy::parser::token::TOK_LOG_OR; }
    int process_log_and() const noexcept { return yy::parser::token::TOK_LOG_AND; }
    int process_assign() const noexcept { return yy::parser::token::TOK_ASSIGN; }
    int process_eq() const noexcept { return yy::parser::token::TOK_EQ; }
    int process_not_eq() const noexcept { return yy::parser::token::TOK_NEQ; }
    int process_less() const noexcept { return yy::parser::token::TOK_LESS; }
    int process_greater() const noexcept { return yy::parser::token::TOK_GREATER; }
    int process_less_or_eq() const noexcept { return yy::parser::token::TOK_LESS_OR_EQ; }
    int process_greater_or_eq() const {
        return yy::parser::token::TOK_GREATER_OR_EQ;
    }
    int process_not() const noexcept { return yy::parser::token::TOK_NOT; }
    int process_left_paren() const noexcept { return yy::parser::token::TOK_LEFT_PAREN; }
    int process_right_paren() const {
        return yy::parser::token::TOK_RIGHT_PAREN;
    }
    int process_left_brace() const noexcept { return yy::parser::token::TOK_LEFT_BRACE; }
    int process_right_brace() const {
        return yy::parser::token::TOK_RIGHT_BRACE;
    }
    int process_semicolon() const noexcept { return yy::parser::token::TOK_SEMICOLON; }
    int process_id() const noexcept { return yy::parser::token::TOK_ID; }
    int process_number() { return yy::parser::token::TOK_NUMBER; }

    int yylex() override;
};

} // namespace language

#endif // FRONTEND_INCLUDE_LEXER_HPP
