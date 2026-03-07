%language "c++"
%defines "parser.hpp"
%locations
%define parse.error detailed
%define api.value.type variant
%define api.parser.class {parser}

%nonassoc PREC_IFX
%nonassoc TOK_ELSE

%lex-param   { language::Lexer *scanner }
%parse-param { language::Lexer *scanner }
%parse-param { language::Node_pool &pool }
%parse-param { language::program_ptr &root }
%parse-param { language::My_parser *my_parser }

%code requires {
  #include <string>
  #include <string_view>
  #include <type_traits>
  #include <utility>
  #include <iostream>
  #include <optional>
  #include <vector>

  #include "config.hpp"
  #include "data_structures/node.hpp"
  #include "data_structures/node_pool.hpp"
  #include "parser/scope.hpp"

  namespace language { class Lexer; }
  namespace language { class My_parser; }

  using language::Node_pool;
  using language::Binary_operators;
  using language::Unary_operators;
  using language::nametable_t;
  using language::name_t;
  using language::name_t_sv;

  template<typename T>
  void push_scope(T* parser, nametable_t&& nametable);

  template<typename T>
  void pop_scope(T* parser);

  template<typename T>
  name_t_sv lookup_in_scopes(T* parser, name_t_sv var_name);

  template<typename T>
  name_t_sv add_var_to_scope(T* parser, name_t_sv var_name);
}

%code {
  #include "config.hpp"
  #include "lexer.hpp"
  #include "error_collector.hpp"
  #include "my_parser.hpp"
  #include <iostream>
  #include <string>

  template<typename T>
  void push_scope(T* parser, nametable_t&& nametable) {
    parser->scopes.push(std::move(nametable));
  }

  template<typename T>
  void pop_scope(T* parser) {
    parser->scopes.pop();
  }

  template<typename T>
  name_t_sv lookup_in_scopes(T* parser, name_t_sv var_name) {
    return parser->scopes.lookup(var_name);
  }

  template<typename T>
  name_t_sv add_var_to_scope(T* parser, name_t_sv var_name) {
    return parser->scopes.add_variable(var_name);
  }

  int yylex(yy::parser::semantic_type* yylval,
            yy::parser::location_type* yylloc,
            language::Lexer*           scanner) {
    int line_before = scanner->get_line();

    auto tt = scanner->yylex();

    yylloc->begin.line = line_before;
    yylloc->begin.column = scanner->get_column() - scanner->get_yyleng();
    yylloc->end.line = scanner->get_line();
    yylloc->end.column = scanner->get_column();

    if (tt == yy::parser::token::TOK_NUMBER)
        yylval->build<int>() = std::stoi(scanner->YYText());

    if (tt == yy::parser::token::TOK_ID)
        yylval->build<std::string>() = scanner->YYText();

    return tt;
  }

  void yy::parser::error(const location& loc, const std::string& msg) {
    my_parser->error_collector.add_error(loc, msg, my_parser->get_line_content(loc.begin.line));
  }
}

/* ________________________Tokens________________________ */
/* --- Keywords --- */
%token TOK_IF            "if"
%token TOK_ELSE          "else"
%token TOK_WHILE         "while"
%token TOK_PRINT         "print"
%token TOK_INPUT         "?"
%token TOK_FUNC          "func"
%token TOK_RETURN        "return"

/* --- Arithmetic operators --- */
%token TOK_PLUS          "+"
%token TOK_MINUS         "-"
%token TOK_MUL           "*"
%token TOK_DIV           "/"
%token TOK_REM_DIV       "%"
%token TOK_AND           "&"
%token TOK_XOR           "^"
%token TOK_OR            "|"

/* --- Logical operators --- */
%token TOK_NOT           "!"
%token TOK_LOG_OR        "||"
%token TOK_LOG_AND       "&&"

/* --- Assignment --- */
%token TOK_ASSIGN        "="

/* --- Comparison operators --- */
%token TOK_EQ            "=="
%token TOK_NEQ           "!="
%token TOK_LESS          "<"
%token TOK_GREATER       ">"
%token TOK_LESS_OR_EQ    "<="
%token TOK_GREATER_OR_EQ ">="

/* --- Parentheses and separators --- */
%token TOK_LEFT_PAREN    "("
%token TOK_RIGHT_PAREN   ")"
%token TOK_LEFT_BRACE    "{"
%token TOK_RIGHT_BRACE   "}"
%token TOK_SEMICOLON     ";"
%token TOK_COMMA         ","
%token TOK_COLON         ":"

/* --- Tokens with semantic values --- */
%token <std::string> TOK_ID     "identifier"
%token <int>         TOK_NUMBER "number"

/* --- End of file --- */
%token TOK_EOF 0
/* ______________________________________________________ */

%type <language::StmtList>                   toplevel_stmt_list
%type <language::Statement_ptr>              toplevel_statement
%type <language::StmtList>                   stmt_list
%type <language::Statement_ptr> statement
%type <language::Statement_ptr> if_stmt while_stmt print_stmt block_stmt empty_stmt return_stmt expr_stmt

%type <language::Expression_ptr>             expression assignment_expr or and bitwise_op equality relational add_sub mul_div unary postfix primary function_expr input_expr
%type <language::Call::ArgExprList>          opt_arg_list arg_list
%type <language::Func::ParamList>            opt_param_list param_list
%type <std::optional<language::name_t_sv>>   opt_func_name

%start program

%%

program        : toplevel_stmt_list TOK_EOF
                {
                  root = pool.make<language::Program>($1);
                }
               ;

toplevel_stmt_list: 
                {
                  $$ = language::StmtList{};
                }
               | toplevel_stmt_list toplevel_statement
                {
                  $1.push_back($2);
                  $$ = $1;
                }
               ;

toplevel_statement: statement
                {
                  $$ = $1;
                }
               | TOK_RIGHT_BRACE
                {
                  error(@1, "unmatched '}'");
                }
                ;

stmt_list: 
                {
                  $$ = language::StmtList{};
                }
               | stmt_list statement
                {
                  $1.push_back($2);
                  $$ = $1;
                }
               ;

statement      : 
                if_stmt
                  { $$ = $1; }
                | while_stmt
                  { $$ = $1; }
                | print_stmt TOK_SEMICOLON
                  { $$ = $1; }
                | return_stmt TOK_SEMICOLON
                  { $$ = $1; }
                | expr_stmt
                  { $$ = $1; }
                | block_stmt
                  { $$ = $1; }
                | empty_stmt
                  { $$ = $1; }
                | error TOK_SEMICOLON
                  {
                    yyerrok;
                    $$ = pool.make<language::Empty_stmt>();
                  }
                ;

empty_stmt     : TOK_SEMICOLON
                {
                  $$ = pool.make<language::Empty_stmt>();
                }
                ;

expr_stmt       : expression TOK_SEMICOLON
                  {
                    $$ = pool.make<language::Expr_stmt>($1);
                  }
                ;

block_stmt     : TOK_LEFT_BRACE
                {
                  push_scope(my_parser, nametable_t{});
                }
                stmt_list
                TOK_RIGHT_BRACE
                {
                  pop_scope(my_parser);
                  $$ = pool.make<language::Block_stmt>($3);
                }
               ;

if_stmt        : TOK_IF TOK_LEFT_PAREN expression TOK_RIGHT_PAREN statement %prec PREC_IFX
                {
                  $$ = pool.make<language::If_stmt>($3, $5);
                }
               | TOK_IF TOK_LEFT_PAREN expression TOK_RIGHT_PAREN statement TOK_ELSE statement
                {
                  $$ = pool.make<language::If_stmt>($3, $5, $7);
                }
               | TOK_IF error TOK_RIGHT_PAREN statement %prec PREC_IFX
                 {
                   yyerrok;
                   $$ = pool.make<language::Empty_stmt>();
                 }
               | TOK_IF TOK_LEFT_PAREN error statement %prec PREC_IFX
                 {
                   yyerrok;
                   $$ = pool.make<language::Empty_stmt>();
                 }
               ;

while_stmt     : TOK_WHILE TOK_LEFT_PAREN expression TOK_RIGHT_PAREN statement
                {
                  $$ = pool.make<language::While_stmt>($3, $5);
                }
               | TOK_WHILE error TOK_RIGHT_PAREN statement
                  {
                    yyerrok;
                    $$ = pool.make<language::Empty_stmt>();
                  }
                | TOK_WHILE TOK_LEFT_PAREN error statement
                  {
                    yyerrok;
                    $$ = pool.make<language::Empty_stmt>();
                  }
               ;

print_stmt     : TOK_PRINT expression
                {
                  $$ = pool.make<language::Print_stmt>($2);
                }
               ;
return_stmt     : TOK_RETURN
                  {
                    $$ = pool.make<language::Return_stmt>(nullptr);
                  }
                | TOK_RETURN expression
                  {
                    $$ = pool.make<language::Return_stmt>($2);
                  }
                ;

expression     : assignment_expr
                {
                  $$ = $1;
                }
              ;

assignment_expr : or { $$ = $1; }
                | TOK_ID TOK_ASSIGN assignment_expr
                  {
                    language::name_t_sv name_sv = lookup_in_scopes(my_parser, $1);
                    if (name_sv.empty())
                      name_sv = add_var_to_scope(my_parser, $1);

                    auto variable = pool.make<language::Variable>(name_sv);
                    $$ = pool.make<language::Assignment_expr>(variable, $3);
                  }
                ;

or            : and { $$ = $1; }
              | or TOK_LOG_OR and
                { $$ = pool.make<language::Binary_operator>(Binary_operators::LogOr, $1, $3); }
              ;

and           : bitwise_op { $$ = $1; }
                | and TOK_LOG_AND bitwise_op
                  { $$ = pool.make<language::Binary_operator>(Binary_operators::LogAnd, $1, $3); }
                ;

bitwise_op     : equality
                  { $$ = $1; }
               | bitwise_op TOK_AND equality
                  { $$ = pool.make<language::Binary_operator>(Binary_operators::And, $1, $3); }
               | bitwise_op TOK_XOR equality
                  { $$ = pool.make<language::Binary_operator>(Binary_operators::Xor, $1, $3); }
               | bitwise_op TOK_OR  equality
                  { $$ = pool.make<language::Binary_operator>(Binary_operators::Or, $1, $3); }
               ;

equality       : relational
                 { $$ = $1; }
               | equality TOK_EQ  relational
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::Eq,  $1, $3); }
               | equality TOK_NEQ relational
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::Neq,  $1, $3); }
               ;

relational     : add_sub
                 { $$ = $1; }
               | relational TOK_LESS          add_sub
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::Less, $1, $3); }
               | relational TOK_LESS_OR_EQ    add_sub
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::LessEq, $1, $3); }
               | relational TOK_GREATER       add_sub
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::Greater, $1, $3); }
               | relational TOK_GREATER_OR_EQ add_sub
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::GreaterEq, $1, $3); }
               ;

add_sub        : mul_div
                 { $$ = $1; }
               | add_sub TOK_PLUS  mul_div
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::Add, $1, $3); }
               | add_sub TOK_MINUS mul_div
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::Sub, $1, $3); }
               ;

mul_div        : unary
                 { $$ = $1; }
               | mul_div TOK_MUL unary
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::Mul, $1, $3); }
               | mul_div TOK_DIV unary
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::Div, $1, $3); }
               | mul_div TOK_REM_DIV unary
                 { $$ = pool.make<language::Binary_operator>(Binary_operators::RemDiv, $1, $3); }
               ;

unary          : TOK_MINUS unary
                { $$ = pool.make<language::Unary_operator>(Unary_operators::Neg, $2); }
               | TOK_PLUS unary
                { $$ = pool.make<language::Unary_operator>(Unary_operators::Plus, $2); }
               | TOK_NOT unary
                { $$ = pool.make<language::Unary_operator>(Unary_operators::Not, $2); }
               | postfix
                { $$ = $1; }
               ;

postfix         : primary
                  {
                    $$ = $1;
                  }
                | postfix TOK_LEFT_PAREN opt_arg_list TOK_RIGHT_PAREN
                  {
                    $$ = pool.make<language::Call>($1, $3);
                  }
                ;

opt_arg_list    :
                  {
                    $$ = language::Call::ArgExprList{};
                  }
                | arg_list
                  {
                    $$ = $1;
                  }
                ;

arg_list        : expression
                  {
                    $$ = language::Call::ArgExprList{};
                    $$.push_back($1);
                  }
                | arg_list TOK_COMMA expression
                  {
                    $1.push_back($3);
                    $$ = $1;
                  }
                ;

primary         : TOK_NUMBER
                  {
                    $$ = pool.make<language::Number>($1);
                  }
                | TOK_ID
                  {
                    language::name_t_sv name_sv = lookup_in_scopes(my_parser, $1);
                    if (name_sv.empty()) {
                      error(@1, std::string("'") + $1 + "' was not declared in this scope");
                      name_sv = add_var_to_scope(my_parser, $1);
                    }

                    $$ = pool.make<language::Variable>(name_sv);
                  }
                | TOK_LEFT_PAREN expression TOK_RIGHT_PAREN
                  {
                    $$ = $2;
                  }
                | function_expr
                  {
                    $$ = $1;
                  }
                | input_expr
                  {
                    $$ = $1;
                  }
                ;

input_expr      : TOK_INPUT
                  {
                    $$ = pool.make<language::Input>();
                  }
                ;

function_expr   : TOK_FUNC
                {
                  push_scope(my_parser, nametable_t{});
                }
                TOK_LEFT_PAREN opt_param_list TOK_RIGHT_PAREN opt_func_name
                TOK_LEFT_BRACE stmt_list TOK_RIGHT_BRACE
                {
                  auto body = pool.make<language::Block_stmt>($8);
                  $$ = pool.make<language::Func>($6, $4, body);
                  pop_scope(my_parser);
                }
              ;

opt_param_list :
                  {
                    $$ = language::Func::ParamList{};
                  }
                | param_list
                  {
                    $$ = $1;
                  }
                ;

param_list      : TOK_ID
                  {
                    $$ = language::Func::ParamList{};
                    $$.push_back(add_var_to_scope(my_parser, $1));
                  }
                | param_list TOK_COMMA TOK_ID
                  {
                    $1.push_back(add_var_to_scope(my_parser, $3));
                    $$ = $1;
                  }
                ;

opt_func_name   :
                  {
                    $$ = std::nullopt;
                  }
                | TOK_COLON TOK_ID
                  {
                    $$ = add_var_to_scope(my_parser, $2);
                  }
                ;

%%
