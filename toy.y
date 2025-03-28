%require "3.2"
%language "c++"

%defines
%define api.namespace {Toy}
%define api.parser.class {Parser}
%define parse.error verbose
%define api.value.automove true

%locations

%code requires {
#include "AST.hpp"
#include <memory>
#include <vector>
#include <string>

namespace Toy{
    class Scanner;
}
}

%parse-param {Toy::Scanner  &scanner}

%code {
    #include "Scanner.hpp"
#undef yylex
#define yylex scanner.yylex
    #include "Logger.hpp"
}


%union {
    int numVal;
    std::string* strVal;
    Toy::ExprAST* exprVal;
    Toy::PrototypeAST* protoVal;
    Toy::FunctionAST* funcVal;
    Toy::IfElseExprAST* ifVal;
    std::vector<std::string>* parmList;
    std::vector<std::unique_ptr<Toy::ExprAST>>* argList;
}


%left ADD SUB            // 优先级最低（加减）
%left MUL DIV            // 优先级中等（乘除）
%left LT LE GT GE EQ NE  // 优先级最高（比较）

%token <numVal> NUMBER
%token <strVal> IDENTIFIER
%token DEF EXTERN IF ELSE
%token LT GT EQ LE GE NE ASSIGN
%token ADD SUB MUL DIV
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMI

%type <exprVal> expr
%type <protoVal> proto
%type <funcVal> function
%type <parmList> parms
%type <argList> args


%%

program:
    | program function {
        LOG_DEBUG() << $2->to_string() << '\n';
    }
    | program EXTERN proto {
        LOG_DEBUG() << $3->to_string() << '\n';
    }
    ;

function:
    DEF IDENTIFIER LPAREN parms RPAREN LBRACE expr RBRACE {
        auto proto = new Toy::PrototypeAST(*$2, *$4);
        $$ = new Toy::FunctionAST(proto, $7);
    }
    ;

proto:
    IDENTIFIER LPAREN parms RPAREN {
        $$ = new Toy::PrototypeAST(*$1, *$3);
    }
    | IDENTIFIER {
        $$ = new Toy::PrototypeAST(*$1, {});
    }
    ;

parms:
    /* empty */ { $$ = new std::vector<std::string>(); }
    | IDENTIFIER {
        $$ = new std::vector<std::string>();
        $$->push_back(*$1);
    }
    | parms COMMA IDENTIFIER {
        $1->push_back(*$3);
    }
    ;

args:
     /* empty */ { $$ = new std::vector<std::unique_ptr<Toy::ExprAST>>(); }
     | expr {
        $$ = new std::vector<std::unique_ptr<Toy::ExprAST>>();
        $$->emplace_back($1);
     }
     | args COMMA expr {
        $1->emplace_back($3);
     }


expr:
    NUMBER         { $$ = new Toy::NumberExprAST($1); }
    | IDENTIFIER     { $$ = new Toy::VariableExprAST(*$1); }
    | expr ADD expr  { $$ = new Toy::BinaryExprAST("+", $1, $3); }
    | expr SUB expr  { $$ = new Toy::BinaryExprAST("-", $1, $3); }
    | expr MUL expr  { $$ = new Toy::BinaryExprAST("*", $1, $3); }
    | expr DIV expr  { $$ = new Toy::BinaryExprAST("/", $1, $3); }
    | expr LT expr   { $$ = new Toy::BinaryExprAST("<", $1, $3); }
    | expr LE expr   { $$ = new Toy::BinaryExprAST("<=", $1, $3); }
    | expr GT expr   { $$ = new Toy::BinaryExprAST(">", $1, $3); }
    | expr GE expr   { $$ = new Toy::BinaryExprAST(">=", $1, $3); }
    | expr EQ expr   { $$ = new Toy::BinaryExprAST("==", $1, $3); }
    | expr NE expr   { $$ = new Toy::BinaryExprAST("!=", $1, $3); }
    | IDENTIFIER LPAREN args RPAREN {
        $$ = new Toy::CallExprAST(*$1, *$3);
    }
    | IF expr LBRACE expr RBRACE ELSE LBRACE expr RBRACE {
        $$ = new Toy::IfElseExprAST($2, $4, $8);
    }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

%%

void Toy::Parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << "error at " << loc << ", message: " << msg << std::endl;
}