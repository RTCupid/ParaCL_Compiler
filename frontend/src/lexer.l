%option yyclass="Lexer"
%option c++
%option nounput noinput

%{
  #include <string>
  #include <iostream>
  #include <fstream>
  #include "parser/lexer.hpp"

  using namespace language;
%}

WHITESPACE    [ \t\r\v]+
ID            [a-zA-Z_][a-zA-Z0-9_]*
NUMBER        [0-9]+
NUMBER1       [1-9]+
ZERO          0
LINE_COMMENT  "//".*
BLOCK_COMMENT "/*"([^*]|\*+[^*/])*\*+"/"
NEWLINE  \n

%%

{WHITESPACE}    { yycolumn += yyleng; }
{NEWLINE}       { ++yylineno; yycolumn = 1; }

{LINE_COMMENT}  { yycolumn += yyleng; }
{BLOCK_COMMENT} { /* skip */ }

"if"            { yycolumn += yyleng; return process_if();       }
"else"          { yycolumn += yyleng; return process_else();     }
"while"         { yycolumn += yyleng; return process_while();    }
"print"         { yycolumn += yyleng; return process_print();    }
"?"             { yycolumn += yyleng; return process_input();    }
"func"          { yycolumn += yyleng; return process_function(); }
"return"        { yycolumn += yyleng; return process_return();   }


"||"             { yycolumn += yyleng; return process_log_or();  }
"&&"             { yycolumn += yyleng; return process_log_and(); }

"!"             { yycolumn += yyleng; return process_not();           }
"=="            { yycolumn += yyleng; return process_eq();            }
"!="            { yycolumn += yyleng; return process_not_eq();        }
"<="            { yycolumn += yyleng; return process_less_or_eq();    }
">="            { yycolumn += yyleng; return process_greater_or_eq(); }
"="             { yycolumn += yyleng; return process_assign();        }

"+"             { yycolumn += yyleng; return process_plus();    }
"-"             { yycolumn += yyleng; return process_minus();   }
"*"             { yycolumn += yyleng; return process_mul();     }
"/"             { yycolumn += yyleng; return process_div();     }
"%"             { yycolumn += yyleng; return process_rem_div(); }
"&"             { yycolumn += yyleng; return process_and();     }
"^"             { yycolumn += yyleng; return process_xor();     }
"|"             { yycolumn += yyleng; return process_or();      }

"<"             { yycolumn += yyleng; return process_less();    }
">"             { yycolumn += yyleng; return process_greater(); }

"("             { yycolumn += yyleng; return process_left_paren();  }
")"             { yycolumn += yyleng; return process_right_paren(); }
"{"             { yycolumn += yyleng; return process_left_brace();  }
"}"             { yycolumn += yyleng; return process_right_brace(); }
";"             { yycolumn += yyleng; return process_semicolon();   }
","             { yycolumn += yyleng; return process_comma();       }
":"             { yycolumn += yyleng; return process_colon();       }

{NUMBER1}{NUMBER}* { yycolumn += yyleng; return process_number(); }
{ZERO}          { yycolumn += yyleng; return process_number();    }

{ID}            { yycolumn += yyleng; return process_id(); }

.               {
                    std::cerr << "Unknown token: '" << yytext << "' at line " << yylineno << std::endl;;
                    return -1;
                }

<<EOF>>         { return 0; }

%%
