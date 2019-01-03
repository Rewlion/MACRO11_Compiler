%{
  #include "ast.h"
  #include <cstdio>

  // Declare stuff from Flex that Bison needs to know about:
  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
  extern int yylineno;
 
  void yyerror(AST::AbstractSyntaxTree* ast, const char *s);
%}

%debug

%parse-param {AST::AbstractSyntaxTree* Ast}

%code requires { #include "ast.h" }

%union {
  int ival;
  float fval;
  char* sval;
  AST::LabelNode*   label;
  AST::OperandNode* operand;
  AST::CommandNode* command_line;
  AST::CommandNode* command_list;
  AST::ProgramNode* program;
}

%token <ival>        INT
%token <fval>        FLOAT
%token <sval>        STRING
%token <ival>        COMMAND
%token <ival>        REGISTER
%token <sval>        LABEL

%type <label>        LABEL_LIST
%type <operand>      OPERAND
%type <command_line> COMMAND_LINE
%type <command_list> COMMAND_LIST
%type <program>      PROGRAM

%token TOKEN_DIRECT_ASSIGN  "=" //=
%token TOKEN_TERM_INDICATOR "%" //%
%token TOKEN_IMMEDIATE_EXPR "#" //#
%token TOKEN_DEFFERRED_EXPR "@" //@
%token TOKEN_LEFT_BRACKET   "(" //(
%token TOKEN_RIGHT_BRACKET  ")" //)
%token TOKEN_COMMA          "," //,
%token TOKEN_PLUS           "+" //+
%token TOKEN_MINUS          "-" //-
%token TOKEN_MUL            "*" //*
%token TOKEN_DIV            "/" // /
%token TOKEN_LOGIC_AND      "&" //&
%token TOKEN_LOGIC_OR       "!" //!
%token DOLLAR "$"

%%

PROGRAM
  : COMMAND_LIST                           { $$ = new AST::ProgramNode($1); Ast->SetProgram($$);}
  ;

COMMAND_LIST
  : COMMAND_LINE COMMAND_LIST              { $$ = $1; $$->Next = $2;}
  | COMMAND_LINE                           { $$ = $1;}
  ;

COMMAND_LINE
  : COMMAND OPERAND "," OPERAND            { $$ = new AST::DoubleOperandCommandNode($1, $2, $4, yylineno);}
  | COMMAND OPERAND                        { $$ = new AST::OneOperandCommandNode($1, $2, yylineno);}
  | COMMAND                                { $$ = new AST::CommandNode($1, yylineno); }
  | LABEL_LIST COMMAND OPERAND "," OPERAND { $$ = new AST::DoubleOperandCommandNode($2, $3, $5, yylineno); $$->Labels = $1;}
  | LABEL_LIST COMMAND OPERAND             { $$ = new AST::OneOperandCommandNode($2, $3, yylineno); $$->Labels = $1;}
  | LABEL_LIST COMMAND                     { $$ = new AST::CommandNode($2, yylineno); $$->Labels = $1; }
  ;  
  
OPERAND
  : REGISTER                               { $$ = new AST::OperandNode(OperandType::Register,  $1, AddressingType::Register);}
  | "(" REGISTER ")" "+"                   { $$ = new AST::OperandNode(OperandType::Register,  $2, AddressingType::AutoIncrement);}
  | "-" "(" REGISTER ")"                   { $$ = new AST::OperandNode(OperandType::Register,  $3, AddressingType::AutoDecrement);}
  | INT "(" REGISTER ")"                   { $$ = new AST::OperandNode(OperandType::Register,  $3, AddressingType::Index, nullptr, $1);}
  | "(" REGISTER ")"                       { $$ = new AST::OperandNode(OperandType::Register,  $2, AddressingType::RegisterDeferred);}
  | "@" REGISTER                           { $$ = new AST::OperandNode(OperandType::Register,  $2, AddressingType::RegisterDeferred);}
  | "@" "(" REGISTER ")" "+"               { $$ = new AST::OperandNode(OperandType::Register,  $3, AddressingType::AutoIncrementDeferred);}
  | "@" "-" "(" REGISTER ")"               { $$ = new AST::OperandNode(OperandType::Register,  $4, AddressingType::AutoDecrementDeferred);}
  | "@" INT "(" REGISTER ")"               { $$ = new AST::OperandNode(OperandType::Register,  $4, AddressingType::IndexDeferred, nullptr, $2);}
  | "#" INT                                { $$ = new AST::OperandNode(OperandType::Number,    $2, AddressingType::AutoIncrement); }
  | "@" "#" INT                            { $$ = new AST::OperandNode(OperandType::Number,    $3, AddressingType::AutoIncrementDeferred);}
  | INT                                    { $$ = new AST::OperandNode(OperandType::Number,    $1, AddressingType::Index, nullptr, $1);}
  | "@" INT                                { $$ = new AST::OperandNode(OperandType::Number,    $2, AddressingType::IndexDeferred, nullptr, $2);}
  | STRING                                 { $$ = new AST::OperandNode(OperandType::LabelName, -1, AddressingType::Label, $1);}
  ;
  
LABEL_LIST
  : LABEL                       { $$ = new AST::LabelNode($1); }
  ;
  
%%

void yyerror(AST::AbstractSyntaxTree* ast, const char* msg) {
  fprintf(stderr, "line %d: %s\n", yylineno, msg);
  exit(-1);
}