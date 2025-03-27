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
}


%union {
    int numVal;
    std::string* strVal;
    Toy::ExprAST* exprVal;
    Toy::PrototypeAST* protoVal;
    Toy::FunctionAST* funcVal;
    std::vector<std::string>* parmList;
    std::vector<std::unique_ptr<Toy::ExprAST>>* argList;
}


%left ADD SUB        // 优先级最低（加减）
%left MUL DIV        // 优先级中等（乘除）
%left LT GT          // 优先级最高（比较）

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
    | program function
    | program extern_decl
    ;

extern_decl:
    EXTERN proto {
    }
    ;

function:
    DEF IDENTIFIER LPAREN parms RPAREN LBRACE expr RBRACE {
        auto proto = new Toy::PrototypeAST(*$2, *$4);
        $$ = new Toy::FunctionAST(
            std::unique_ptr<Toy::PrototypeAST>(proto),
            std::unique_ptr<Toy::ExprAST>($7)
        );
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
     }
     | args COMMA expr {

     }

expr:
    NUMBER         { $$ = new Toy::NumberExprAST($1); }
    | IDENTIFIER     { $$ = new Toy::VariableExprAST(*$1); }
    | expr ADD expr  { $$ = new Toy::BinaryExprAST('+', $1, $3); }
    | expr SUB expr  { $$ = new Toy::BinaryExprAST('-', $1, $3); }
    | expr MUL expr  { $$ = new Toy::BinaryExprAST('*', $1, $3); }
    | expr DIV expr  { $$ = new Toy::BinaryExprAST('/', $1, $3); }
    | expr LT expr   { $$ = new Toy::BinaryExprAST('<', $1, $3); }
    | expr GT expr   { $$ = new Toy::BinaryExprAST('>', $1, $3); }
    | IDENTIFIER LPAREN args RPAREN {
        // $$ = new Toy::CallExprAST(*$1, *$3);
    }
    | IF expr LBRACE expr RBRACE ELSE LBRACE expr RBRACE {
        // 注意：这里简化了if-else的AST表示，实际需要更完整的处理
    }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

%%

void Toy::Parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << "error at " << loc << ", message: " << msg << std::endl;
}