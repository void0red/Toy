%option c++
%option noyywrap
%option yyclass="Toy::Scanner"
%option debug

%{
#include "AST.hpp"
#include "Scanner.hpp"
#include "toy.tab.hpp"
using TOKEN = Toy::Parser::token;
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);
%}

%%


"def"      { return TOKEN::DEF; }
"extern"   { return TOKEN::EXTERN; }
"if"       { return TOKEN::IF; }
"else"     { return TOKEN::ELSE; }

"=="       { return TOKEN::EQ; }
"!="       { return TOKEN::NE; }
"<"        { return TOKEN::LT; }
"<="       { return TOKEN::LE; }
">"        { return TOKEN::GT; }
">="       { return TOKEN::GE; }

"+"        { return TOKEN::ADD; }
"-"        { return TOKEN::SUB; }
"*"        { return TOKEN::MUL; }
"/"        { return TOKEN::DIV; }

"="        { return TOKEN::ASSIGN; }
"{"        { return TOKEN::LBRACE; }
"}"        { return TOKEN::RBRACE; }
"("        { return TOKEN::LPAREN; }
")"        { return TOKEN::RPAREN; }

","        { return TOKEN::COMMA; }
";"        { return TOKEN::SEMI; }


[a-zA-Z_][a-zA-Z0-9_]* {
    yylval->strVal = new std::string(yytext);
    return TOKEN::IDENTIFIER;
}

[0-9]+     {
    yylval->numVal = std::stoi(yytext);
    return TOKEN::NUMBER;
}

[ \t\r]  { }

\n {
    loc->lines();
}

.          {
    std::cerr << "Invalid character: " << yytext << std::endl;
    yyterminate();
}

%%