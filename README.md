<div align="center">

# Implementation of Compiler for Programming Language ParaCL in C++
  ![C++](https://img.shields.io/badge/C++-23-blue?style=for-the-badge&logo=cplusplus)
  ![CMake](https://img.shields.io/badge/CMake-3.20+-green?style=for-the-badge&logo=cmake)
  ![Testing](https://img.shields.io/badge/Google_Test-Framework-red?style=for-the-badge&logo=google)

</div>

- This project presents the implementation of the `ParaCL` programming language from the C++ course by K.I. Vladimirov.

## README in other languages 

1. [Russian](/README-R.md)
2. [English](/README.md)

## Table of Contents
Introduction:
- [Running the program](#running-the-program)
- [Introduction](#introduction)
- [Methodology](#methodology)

Language usage instructions:
- [Language capabilities description](#language-capabilities-description)
- [Compilation error display](#compilation-error-display)
- [Variables and numbers](#variables-and-numbers)
- [Single-line and multi-line comments](#single-line-and-multi-line-comments)
- [Input and output operators](#input-and-output-operators)
- [Assignment and chained assignment](#assignment-and-chained-assignment)
- [Conditionals and loops](#conditionals-and-loops)
- [Local variables and scope](#local-variables-and-scope)
- [Logical operators](#logical-operators)
- [Arithmetic and bitwise operators](#arithmetic-and-bitwise-operators)

Frontend implementation:
- [Lexical analyzer implementation](#lexical-analyzer-implementation)
- [Syntax analyzer implementation](#syntax-analyzer-implementation)
- [Designing data structures for program storage](#designing-data-structures-for-program-storage)
- [Static factory implementation for AST node generation](#static-factory-implementation-for-ast-node-generation)
- [Error collector implementation](#error-collector-implementation)
- [Scope implementation](#scope-implementation)
- [Simulator implementation](#simulator-implementation)

Additional:
- [Using dump](#using-dump)
- [Project structure](#project-structure)
- [Project authors](#project-authors)

### Running the program
Repository cloning, build and compilation is performed using the following commands:

```
git clone [https://github.com/RTCupid/Biba_Boba_Buba_Language.git](https://github.com/RTCupid/Biba_Boba_Buba_Language.git)
cd Biba_Boba_Buba_Language
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Program execution is performed in the following format:
```
./build/frontend/frontend <program filename>
```

## Introduction
Developing a programming language is a fundamental task in computer science that allows practical investigation of computation principles. Creating a language with C-like syntax provides better understanding of compiler architecture. This process reveals the inner logic of translating high-level constructs into intermediate representations.

Manual implementation of lexical and syntax analyzers involves significant difficulties. This approach requires writing and debugging low-level code, which is particularly problematic when modifying the grammar. Handling operator precedence and associativity becomes a non-trivial task, making language support extremely labor-intensive.

The use of `Flex` and `Bison` tools enables automation of analyzer creation. `Flex` generates an efficient scanner based on regular expressions, and `Bison` builds an LALR(1) parser that performs syntax analysis with one-token lookahead. This approach significantly accelerates development while ensuring reliability and ease of grammar modification.

## Methodology
The `EBNF` format [1] is suitable for describing grammar. `Flex` and `Bison` can be used for lexical and syntax analyzer generation. An interpreter can be written to execute the program, which will traverse the `AST` using the `Visitor` abstraction and simulate program execution.

## Language capabilities description

A grammar for the target programming language has been developed. Below is its description in a format similar to `EBNF` [1]:

<details>
<summary>Grammar</summary>
  
```
Program        ::= StmtList EOF

StmtList       ::= /* empty */ |  StmtList Statement 

Statement      ::= AssignmentStmt ';' | InputStmt ';' | IfStmt | WhileStmt | PrintStmt ';' | BlockStmt | ';'

BlockStmt      ::= '{' StmtList '}'
AssignmentStmt ::= Var '=' Expression
InputStmt      ::= Var '=' '?'
IfStmt         ::= 'if'    '(' Expression ')' Statement [ 'else' Statement ]
WhileStmt      ::= 'while' '(' Expression ')' Statement
PrintStmt      ::= 'print' Expression

Expression     ::= AssignmentExpr
AssignmentExpr ::= Or | Var '=' AssignmentExpr
Or             ::= And | Or '||' And
And            ::= BitwiseOp | And '&&' BitwiseOp
BitwiseOp      ::= Equality | BitwiseOp '&' Equality | BitwiseOp '^' Equality | BitwiseOp '|'  Equality
Equality       ::= Relational ( ( '==' | '!=' ) Relational )*
Relational     ::= AddSub ( ( '<' | '>' | '<=' | '>=' ) AddSub )*
AddSub         ::= MulDiv ( ( '+' | '-' ) MulDiv )*
MulDiv         ::= Unary  ( ( '*' | '/' ) Unary )*
Unary          ::= '-' Unary | '+' Unary | '~' Unary | Primary
Primary        ::= '(' Expression ')' | Var | Number

Var            ::= [A-Za-z_][A-Za-z0-9_]*
Number         ::= [1-9][0-9]* | '0'
EOF            ::= __end_of_file__
```

</details>

## Compilation error display
The frontend provides detailed `error output` that contains information about the file, line and position where the error occurred. The error description is also provided, the entire line with the error is displayed, and the token that does not match the language rules is highlighted:

<details>
<summary>Simple example of error output</summary>

<div align="center">
  <img src="img/simple_errors_1.png" alt="simple_errors_1" width="1200">
</div>

</details>

During syntax analysis, all possible errors are collected and displayed. For errors that occurred as a result of violating grammar rules, a correction is suggested in accordance with the rules:

<details>
<summary>Example of multiple error output and correction suggestion</summary>

<div align="center">
  <img src="img/simple_errors_2.png" alt="simple_errors_2" width="1200">
</div>

</details>

## Variables and numbers
`Variables and numbers` in the language have integer values. Variables must start with a letter `[a-zA-Z_]`, then can be followed by any number of letters or digits `[a-zA-Z0-9_]`. Numbers start with a non-zero digit `[1-9]`, then can be followed by any digits `[0-9]`. The number `0` is treated separately.

## Single-line and multi-line comments
The language supports `two types of comments: single-line and multi-line`. Single-line comments start with `//` and end with a `line break`, multi-line comments start with `/*` and end with `*/`:

<details>
<summary>Comments</summary>

```C
/* Program to calculate the trajectory
   of landing on the surface of a star */

T = 38966; // K, star temperature
```

</details>

## Input and output operators
`Input and output operators` have been implemented. The output operator is characterized by the keyword `print` and expects an expression followed by `;`. The result of the expression is output to `std::cout`. The input operator is written using the `?` symbol. It can be used anywhere a number is suitable. During program execution, the number will be taken from `std::cin`:

<details>
<summary>Example: input and output operators</summary>

```C
a = ?;
print a;
```

After running the program, user input will be requested, and the entered number will be output to `std::cout`.

</details>

## Assignment and chained assignment
The `assignment operator`, which allows declaring and initializing variables, can be used both as a separate statement and as an expression whose result is the value assigned to the variable. This allows assignment to be used in other expressions and also makes it possible to implement `chained assignment`:

<details>
<summary>Example: assignment and chained assignment</summary>

```C
i = 2 + (j = 1 + (k = 1));
print k; // 1
print j; // 2
print i; // 4
```

</details>

## Conditionals and loops
The language implements conditionals and loops. Conditionals allow you to check the result of an expression and perform actions based on it. The keyword `if` is used, followed by an expression in parentheses. Then either a single statement is written, which will execute if the expression result is non-zero, or a block of statements in curly braces. After that, an `else` block can be added, which will execute if the result is zero. The conditional structure can be written as: `'if' '(' Expression ')' Statement [ 'else' Statement ]`.

<details>
<summary>Example: conditional</summary>

```C
a = ?;

if (a) {
  print 100/a;
} else {
  print 999;
  print -999;
}
```

If a non-zero number comes to `std::cin`, then the result of the division `100/a` will be output to `std::cout`. Otherwise, two numbers `999` and `-999` will be output.

</details>

Loops are constructed similarly: the keyword `while` is used with the following expression in parentheses, followed by a single statement or a block of statements that repeat while the expression in parentheses is non-zero. The loop structure looks as follows: `'while' '(' Expression ')' Statement`.

<details>
<summary>Example: loop</summary>

```C
a = ?;
while (a) {
  print a;
  a = ?;
}
```

While non-zero numbers are supplied to `std::cin`, these numbers are redirected to `std::cout`. When `0` is entered, the loop will terminate.

</details>

## Local variables and scope
The frontend handles `variable scope`. A global scope exists from the beginning of the program. When entering a new code block, a corresponding scope is created, and outer scopes are accessible from within. When exiting a code block, its scope is deleted along with the variables declared in it:

<details>
<summary>Example: local variables</summary>

```C
a = 5;

if (a) {
  b = 999;
  print a; // correct
  print b; // correct
}

print b; // error, variable b is not defined in this scope
```

</details>

## Logical operators
The language supports logical operators: `&&` - logical AND, `||` - logical OR, `!` - logical NOT. These operators are applied to expressions, and their result is `1` or `0` depending on the combination of zero and non-zero expressions. The first two are binary, the last is unary.

<details>
<summary>Example: logical operators</summary>

```C
a = 4;
b = 5;

if (a == 4 && b == 5) {
    print 1; // correct
} else {
    print 0;
}

if (a == 5  /* incorrect */ || !(b == 4) /* correct, because b is not equal to 4*/) {
    print 1; // correct
} else {
    print 0;
}
```

The result will be: `11`.

</details>

## Arithmetic and bitwise operators
The language also includes all arithmetic operators: `+`, `-`, `*`, `/`, unary `-`, unary `+`, and bitwise operators: `|`, `^`, `&`, `~`, as well as comparison operators: `>`, `<`, `=`, `>=`, `<=`, `==`, `!=`.

<details>
<summary>Example: arithmetic operators - calculating Fibonacci numbers</summary>

```C
fst = 0; 
snd = 1;
number = ?;

while (number > 1) {
    tmp = fst;
    fst = snd;
    snd = snd + tmp;

    number = number - 1;
}

print snd;
```

The result will be the n-th Fibonacci number, where n is the number from `std::cin`.

</details>

<details>
<summary>Example: bitwise operators - basic operations</summary>

```C
a = 12; // 1100 in binary
b = 10; // 1010 in binary

result_and = a & b;
expected_and = 8; // 1000 in binary
if (result_and == expected_and) {
    print 1;
} else {
    print 0;
}

result_or = a | b;
expected_or = 14; // 1110 in binary
if (result_or == expected_or) {
    print 1;
} else {
    print 0;
}

result_xor = a ^ b;
expected_xor = 6; // 0110 in binary
if (result_xor == expected_xor) {
    print 1;
} else {
    print 0;
}
```

The result will be: `111`.

</details>


## Lexical analyzer implementation
Lexical analyzer generation using `Flex` has been implemented (see [lexer.l](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/src/lexer.l)).

The following have been defined:

<details>
<summary>lexical constructs and rules for their processing</summary>

```l
WHITESPACE    [ \\t\\r\\v]+
ID            [a-zA-Z_][a-zA-Z0-9_]*
NUMBER        [0-9]+
NUMBER1       [1-9]+
ZERO          0
LINE_COMMENT  "//".*
BLOCK_COMMENT "/*"([^*]|\\*+[^*/])*\\*+"/"
NEWLINE  \\n

%%

{WHITESPACE}    { yycolumn += yyleng; }
{NEWLINE}       { ++yylineno; yycolumn = 1; }

{LINE_COMMENT}  { yycolumn += yyleng; }
{BLOCK_COMMENT} { /* skip */ }

"if"            { yycolumn += yyleng; return process_if();   }
"else"          { yycolumn += yyleng; return process_else(); }
"while"         { yycolumn += yyleng; return process_while(); }
"print"         { yycolumn += yyleng; return process_print(); }
"?"             { yycolumn += yyleng; return process_input(); }

"||"             { yycolumn += yyleng; return process_log_or(); }
"&&"             { yycolumn += yyleng; return process_log_and(); }

"!"             { yycolumn += yyleng; return process_not(); }
"=="            { yycolumn += yyleng; return process_eq(); }
"!="            { yycolumn += yyleng; return process_not_eq(); }
"<="            { yycolumn += yyleng; return process_less_or_eq(); }
">="            { yycolumn += yyleng; return process_greater_or_eq(); }
"="             { yycolumn += yyleng; return process_assign(); }

"+"             { yycolumn += yyleng; return process_plus(); }
"-"             { yycolumn += yyleng; return process_minus(); }
"*"             { yycolumn += yyleng; return process_mul(); }
"/"             { yycolumn += yyleng; return process_div(); }
"%"             { yycolumn += yyleng; return process_rem_div(); }
"&"             { yycolumn += yyleng; return process_and(); }
"^"             { yycolumn += yyleng; return process_xor(); }
"|"             { yycolumn += yyleng; return process_or(); }

"<"             { yycolumn += yyleng; return process_less(); }
">"             { yycolumn += yyleng; return process_greater(); }

"("             { yycolumn += yyleng; return process_left_paren(); }
")"             { yycolumn += yyleng; return process_right_paren(); }
"{"             { yycolumn += yyleng; return process_left_brace(); }
"}"             { yycolumn += yyleng; return process_right_brace(); }
";"             { yycolumn += yyleng; return process_semicolon(); }

{NUMBER1}{NUMBER}* { yycolumn += yyleng; return process_number(); }
{ZERO}          { yycolumn += yyleng; return process_number(); }

{ID}            { yycolumn += yyleng; return process_id(); }

.               {
                    std::cerr << "Unknown token: '" << yytext << "' at line " << yylineno << std::endl;;
                    return -1;
                }

<<EOF>>         { return 0; }

%%
```

</details>

Token processing functions are defined in the `Lexer` class, which inherits from `yyFlexLexer` (see [lexer.hpp](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/include/lexer.hpp)). They return the corresponding parser token generated by `Bison`. This is done for joint operation of `Bison` and `Flex`.

Functions for obtaining token location have been added to the `Lexer` class:

<details>
<summary>functions for obtaining token location</summary>

```C++
int get_line() const { return yylineno; }

int get_column() const { return yycolumn; }

int get_yyleng() const { return yyleng; }
```

</details>

## Syntax analyzer implementation
For syntax analysis, the `My_parser` class has been added (see [my_parser.hpp](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/include/my_parser.hpp)). It inherits from `yy::parser`, which is generated using `Bison` (see [parser.y](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/src/parser.y)), and contains the following fields and methods:

<details>
<summary>My_parser class</summary>
  
```C++
class My_parser final : public yy::parser {
  private:
    Lexer *scanner_;
    std::unique_ptr<Program> root_;
    std::vector<std::string> source_lines_;

  public:
    Error_collector error_collector;
    Scope scopes;

    My_parser(Lexer *scanner, std::unique_ptr<language::Program> &root,
              const std::string &program_file)
        : yy::parser(scanner, root, this), scanner_(scanner),
          root_(std::move(root)), error_collector(program_file) {
        read_source(program_file);
    }
    ...
};
```

</details>

The function through which the parser interacts with the lexer:

<details>
<summary>yylex function</summary>

```C++
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
```

For numbers and variables, the value is saved in `yylval`. In other cases, the token type is returned.

</details>

During syntax analysis, an `AST` (abstract syntax tree) is built. By introducing new syntax analysis rules, a hierarchy of execution order has been implemented.

## Designing data structures for program storage
For the representation of a structured program (a program containing only conditionals and loops, without goto), a tree structure - `AST` (abstract syntax tree) - has been chosen. For storing such data, the following `class hierarchy` has been implemented (see [node.hpp](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/include/node.hpp)). A common parent node - `Node` - has been created for all node classes with a virtual destructor and a virtual `accept` method for using the `Visitor` pattern:

<details>
<summary>Node class</summary>

```C++
class Node {
  public:
    virtual ~Node() = default;
    virtual void accept(ASTVisitor &visitor) = 0;
};
```

</details>

Two classes inherit from `Node`: `Statement` and `Expression`. The first is for implementing single statements, the second for expressions:

<details>
<summary>abstract Statement and Expression classes</summary>

```C++
class Statement : public Node {};
class Expression : public Node {};
```

</details>

Then, classes for all operators that have expression meaning inherit from `Expression`, and classes for all other statements inherit from `Statement`: output operators, conditionals and loops, as well as empty statements and statement blocks. For example, the class for the `while` loop operator looks as follows:

<details>
<summary>While_stmt class</summary>

```C++
class While_stmt : public Statement {
  private:
    Expression_ptr condition_;
    Statement_ptr body_;

  public:
    While_stmt(Expression_ptr condition, Statement_ptr body)
        : condition_(std::move(condition)), body_(std::move(body)) {}

    Expression &get_condition() { return *condition_; }
    Statement &get_body() { return *body_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};
```

</details>

It publicly inherits from Statement and overrides the accept function for the `Visitor` inside itself.

For example, the class for binary operators, which inherits from `Expression`, is implemented as follows:

<details>
<summary>Binary_operator class</summary>

```C++
class Binary_operator : public Expression {
  private:
    Binary_operators op_;
    Expression_ptr left_;
    Expression_ptr right_;

  public:
    Binary_operator(Binary_operators op, Expression_ptr left,
                    Expression_ptr right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {}

    Binary_operators get_operator() const { return op_; }
    Expression &get_left() { return *left_; }
    const Expression &get_left() const { return *left_; }
    Expression &get_right() { return *right_; }
    const Expression &get_right() const { return *right_; }

    void accept(ASTVisitor &visitor) override { visitor.visit(*this); }
};
```

</details>

The `Variable` and `Number` classes also inherit from `Expression`. The exception is the assignment operator, which is divided into two classes - one inheriting from `Statement`, the other from `Expression`, because it can be used in different contexts with different meanings.

## Static factory implementation for AST node generation
To generate `AST` from nodes with the classes discussed above, a `static factory` has been used (see [ast_factory.hpp](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/include/ast_factory.hpp)). This method allows the person developing the syntax analyzer to not think about the internal structure of the tree, but to call the corresponding methods with named arguments:

<details>
<summary>AST_Factory class</summary>

```C++
class AST_Factory final {
  public:
    static std::unique_ptr<Program> makeProgram(StmtList stmts) {
        return std::make_unique<Program>(std::move(stmts));
    }

    static Statement_ptr makeEmpty() { return std::make_unique<Empty_stmt>(); }

    static Statement_ptr makeBlock(StmtList stmts = {}) {
        return std::make_unique<Block_stmt>(std::move(stmts));
    }

    static Statement_ptr makeWhile(Expression_ptr condition,
                                   Statement_ptr body) {
        return std::make_unique<While_stmt>(std::move(condition),
                                            std::move(body));
    }

    static Statement_ptr makeIf(Expression_ptr condition,
                                Statement_ptr then_branch,
                                Statement_ptr else_branch = nullptr) {
        return std::make_unique<If_stmt>(std::move(condition),
                                         std::move(then_branch),
                                         std::move(else_branch));
    }

    static Expression_ptr makeAssignmentExpr(Variable_ptr variable,
                                             Expression_ptr expression) {
        return std::make_unique<Assignment_expr>(std::move(variable),
                                                 std::move(expression));
    }
...
};
```

</details>


## Error collector implementation
To collect errors, an `Error_collector` has been implemented (see [error_collector.hpp](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/include/error_collector.hpp)).

Inside, it stores `std::vector` with information about each error:

<details>
<summary>Error_info structure</summary>

```C++  
struct Error_info {
  const std::string program_file_;
  const yy::location loc_;
  const std::string msg_;
  const std::string line_with_error_;

  Error_info(const std::string program_file, const yy::location &loc,
             const std::string &msg, const std::string &line_with_error)
      : program_file_(program_file), loc_(loc), msg_(msg),
        line_with_error_(line_with_error) {}

  Error_info(const std::string program_file, const yy::location &loc,
             const std::string &msg)
      : program_file_(program_file), loc_(loc), msg_(msg) {}

  void print(std::ostream &os) const {
      ...
  }
};
```

</details>

It also contains methods for adding and displaying errors:

<details>
<summary>Error_collector methods</summary>

```C++
void add_error(const yy::location &loc, const std::string &msg,
               const std::string &line_with_error) {
    errors_.push_back(Error_info{program_file_, loc, msg, line_with_error});
}

void add_error(const yy::location &loc, const std::string &msg) {
    errors_.push_back(Error_info{program_file_, loc, msg});
}

bool has_errors() const { return !errors_.empty(); }

void print_errors(std::ostream &os) const {
    if (!errors_.empty())
        for (auto &error : errors_)
            error.print(os);
}
```

</details>

`My_parser` contains an `Error_collector` field, which allows errors to be added directly during syntax analysis using the `error` function:

<details>
<summary>error function</summary>

```C++
void yy::parser::error(const location& loc, const std::string& msg) {
    my_parser->error_collector.add_error(loc, msg, my_parser->get_line_content(loc.begin.line));
  }
```

</details>

## Scope implementation
To support local variables, a `Scope` class has been added (see [scope.hpp](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/include/scope.hpp)). It stores a vector of name tables for each scope and has methods for adding new scopes and deleting the most recently added scopes, as well as for searching for variables by name in all scopes accessible at a given point in the program:

<details>
<summary>Scope class</summary>

```C++
class Scope final {
  private:
    std::vector<nametable_t> scopes_;

  public:
    Scope() {
        push(nametable_t{}); // add global scope
    }

    void push(nametable_t nametable) { scopes_.push_back(nametable); }

    void pop() { scopes_.pop_back(); }

    void add_variable(name_t &var_name, bool defined) {
        assert(!scopes_.empty());
        scopes_.back().emplace(var_name, defined);
    }

    bool find(name_t &var_name) const {
        for (auto it = scopes_.rbegin(), last_it = scopes_.rend();
             it != last_it; ++it) {
            auto var_iter = it->find(var_name);
            if (var_iter != it->end())
                return true;
        }

        return false;
    }
};
```

</details>

An instance of the `Scope` class is stored in the `My_parser` class and is used to check the presence of a variable in the scope during syntax analysis.

## Simulator implementation
To simulate program execution, a `Simulator` class has been implemented (see [simulator.hpp](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/include/simulator.hpp)), which inherits from the abstract `ASTVisitor` class:

<details>
<summary>ASTVisitor class</summary>
  
```C++
class ASTVisitor {
  public:
    virtual ~ASTVisitor() = default;

    virtual void visit(Program &node) = 0;
    virtual void visit(Block_stmt &node) = 0;
    virtual void visit(Empty_stmt &node) = 0;
    virtual void visit(Assignment_stmt &node) = 0;
    virtual void visit(Assignment_expr &node) = 0;
    virtual void visit(Input &node) = 0;
    virtual void visit(If_stmt &node) = 0;
    virtual void visit(While_stmt &node) = 0;
    virtual void visit(Print_stmt &node) = 0;
    virtual void visit(Binary_operator &node) = 0;
    virtual void visit(Unary_operator &node) = 0;
    virtual void visit(Number &node) = 0;
    virtual void visit(Variable &node) = 0;
};
```

</details>

In the `Simulator` class, virtual functions of `ASTVisitor` are overridden, and a function for evaluating expressions is introduced, which uses a special `ExpressionEvaluator` class (see [expr_evaluator.hpp](https://github.com/RTCupid/Super_Biba_Boba_Language/blob/main/frontend/include/expr_evaluator.hpp)):

<details>
<summary>evaluate_expression function</summary>

```C++
number_t Simulator::evaluate_expression(Expression &expression) {
    ExpressionEvaluator evaluator(*this);
    expression.accept(evaluator);
    return evaluator.get_result();
}
```

</details>

`ExpressionEvaluator` specializes only in expression evaluation, contains the `result_` field for storing the expression result, and `simulator_` - a reference to the simulator from which it was called, to have access to the name table.

## Using dump
To enable the graph dump option for the tree, you need to set the `-GRAPH_DUMP` flag, which is disabled by default:
```bash
cmake -S . -B build -DGRAPH_DUMP=ON
```
The constructed `AST` tree can be viewed in graphical representation using `graphviz`. To generate an image, you can enter:
```bash
dot dump/dump.gv -Tsvg -o dump/dump.svg
```
This produces the following tree representation:

<details>
<summary>example of generated AST</summary>
  
<div align="center">
  <img src="img/graph_dump.svg" alt="Dump Banner" width="1200">
</div>

</details>

## Project structure

<details>
<summary>Project structure</summary>

```
├── build
├── CMakeLists.txt
├── contribution_guidelines.md
├── frontend
│   ├── CMakeLists.txt
│   ├── include
│   │   ├── config.hpp
│   │   ├── driver.hpp
│   │   ├── dump_path_gen.hpp
│   │   ├── error_collector.hpp
│   │   ├── expr_evaluator.hpp
│   │   ├── lexer.hpp
│   │   ├── my_parser.hpp
│   │   ├── node_pool.hpp
│   │   ├── node.hpp
│   │   ├── scope.hpp
│   │   └── simulator.hpp
│   ├── src
│   │   ├── driver.cpp
│   │   ├── expr_evaluator.cpp
│   │   ├── graph_dump.cpp
│   │   ├── lexer.l
│   │   ├── main.cpp
│   │   ├── parser.y
│   │   └── simulator.cpp
│   └── tests
│       ├── CMakeLists.txt
│       ├── end_to_end
│           └── ...
│       └── unit
│           └── ...
├── img
│   └── ...
├── LICENSE
├── README.md
└── README-R.md

```

</details>

## Project authors

<div align="center">

  <a href="https://github.com/RTCupid">
    <img src="https://raw.githubusercontent.com/BulgakovDmitry/3D_triangles/main/img/A.jpeg" width="160" height="160" style="border-radius: 50%;">
  </a>
  <a href="https://github.com/BulgakovDmitry">
    <img src="https://raw.githubusercontent.com/BulgakovDmitry/3D_triangles/main/img/D.jpeg" width="160" height="160" style="border-radius: 50%;">
  </a>
  <a href="https://github.com/lavrt">
    <img src="https://raw.githubusercontent.com/RTCupid/Biba_Boba_Buba_Language/main/img/lesha.png" width="160" height="160" style="border-radius: 50%;">
  </a>
  <br>
  <a href="https://github.com/RTCupid"><strong>@RTCupid, </strong></a>
  <a href="https://github.com/BulgakovDmitry"><strong>@BulgakovDmitry, </strong></a>
  <a href="https://github.com/lavrt"><strong>@lavrt</strong></a>
  <br>
</div>

## 📚 References
1. Extended Backus-Naur Form [Electronic resource]: article. - [https://divancoder.ru/2017/06/ebnf/](https://divancoder.ru/2017/06/ebnf/) (accessed May 21, 2025)
