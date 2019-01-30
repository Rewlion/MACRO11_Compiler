%{
  #include "Macro11Common.h"
  #include "macro11.tab.h"
  
  #include <cstring>
  #include <iostream>
  
  namespace AST
  {
    class AbstractSyntaxTree;
  }
  
  extern int yylex();  
  extern void yyerror(AST::AbstractSyntaxTree* ast, const char* yytext);
  
  int yylineno = 1;

%}
%option noyywrap
%%
[ \t]*          ;
[\n]            { ++yylineno;}

[-+]?[0-9]+     { yylval.ival = atoi(yytext); return INT; }
[rR][0-7]       { yylval.ival = yytext[1] - '0'; return REGISTER;}
=               { return TOKEN_DIRECT_ASSIGN;}
%               { return TOKEN_TERM_INDICATOR;}
#               { return TOKEN_IMMEDIATE_EXPR;}
@               { return TOKEN_DEFFERRED_EXPR;}
\(              { return TOKEN_LEFT_BRACKET;}
\)              { return TOKEN_RIGHT_BRACKET;}
,               { return TOKEN_COMMA;}
\+              { return TOKEN_PLUS;}
-               { return TOKEN_MINUS;}
\*              { return TOKEN_MUL;}
\/              { return TOKEN_DIV;}
&               { return TOKEN_LOGIC_AND;}
!               { return TOKEN_LOGIC_OR;}
ADC             { yylval.ival = OPCODE_ADC   ; return COMMAND;}
ADCB            { yylval.ival = OPCODE_ADCB  ; return COMMAND;} 
ADD             { yylval.ival = OPCODE_ADD   ; return COMMAND;} 
ASH             { yylval.ival = OPCODE_ASH   ; return COMMAND;} 
ASHC            { yylval.ival = OPCODE_ASHC  ; return COMMAND;} 
ASL             { yylval.ival = OPCODE_ASL   ; return COMMAND;} 
ASLB            { yylval.ival = OPCODE_ASLB  ; return COMMAND;} 
ASR             { yylval.ival = OPCODE_ASR   ; return COMMAND;} 
ASRB            { yylval.ival = OPCODE_ASRB  ; return COMMAND;} 
BCC             { yylval.ival = OPCODE_BCC   ; return COMMAND;} 
BCS             { yylval.ival = OPCODE_BCS   ; return COMMAND;} 
BEQ             { yylval.ival = OPCODE_BEQ   ; return COMMAND;} 
BGE             { yylval.ival = OPCODE_BGE   ; return COMMAND;} 
BGT             { yylval.ival = OPCODE_BGT   ; return COMMAND;} 
BHI             { yylval.ival = OPCODE_BHI   ; return COMMAND;} 
BHIS            { yylval.ival = OPCODE_BHIS  ; return COMMAND;} 
BIC             { yylval.ival = OPCODE_BIC   ; return COMMAND;} 
BICB            { yylval.ival = OPCODE_BICB  ; return COMMAND;} 
BIS             { yylval.ival = OPCODE_BIS   ; return COMMAND;} 
BISB            { yylval.ival = OPCODE_BISB  ; return COMMAND;} 
BIT             { yylval.ival = OPCODE_BIT   ; return COMMAND;} 
BITB            { yylval.ival = OPCODE_BITB  ; return COMMAND;} 
BLE             { yylval.ival = OPCODE_BLE   ; return COMMAND;} 
BLO             { yylval.ival = OPCODE_BLO   ; return COMMAND;} 
BLOS            { yylval.ival = OPCODE_BLOS  ; return COMMAND;} 
BLT             { yylval.ival = OPCODE_BLT   ; return COMMAND;} 
BMI             { yylval.ival = OPCODE_BMI   ; return COMMAND;} 
BNE             { yylval.ival = OPCODE_BNE   ; return COMMAND;} 
BPL             { yylval.ival = OPCODE_BPL   ; return COMMAND;} 
BPT             { yylval.ival = OPCODE_BPT   ; return COMMAND;} 
BR              { yylval.ival = OPCODE_BR    ; return COMMAND;} 
BVC             { yylval.ival = OPCODE_BVC   ; return COMMAND;} 
BVS             { yylval.ival = OPCODE_BVS   ; return COMMAND;} 
CALL            { yylval.ival = OPCODE_CALL  ; return COMMAND;} 
CALLR           { yylval.ival = OPCODE_CALLR ; return COMMAND;}
CCC             { yylval.ival = OPCODE_CCC   ; return COMMAND;}
CLC             { yylval.ival = OPCODE_CLC   ; return COMMAND;}
CLN             { yylval.ival = OPCODE_CLN   ; return COMMAND;}
CLR             { yylval.ival = OPCODE_CLR   ; return COMMAND;}
CLRB            { yylval.ival = OPCODE_CLRB  ; return COMMAND;}
CLV             { yylval.ival = OPCODE_CLV   ; return COMMAND;}
CLZ             { yylval.ival = OPCODE_CLZ   ; return COMMAND;}
CMP             { yylval.ival = OPCODE_CMP   ; return COMMAND;}
CMPB            { yylval.ival = OPCODE_CMPB  ; return COMMAND;}
COM             { yylval.ival = OPCODE_COM   ; return COMMAND;}
COMB            { yylval.ival = OPCODE_COMB  ; return COMMAND;}
DEC             { yylval.ival = OPCODE_DEC   ; return COMMAND;}
DECB            { yylval.ival = OPCODE_DECB  ; return COMMAND;}
DIV             { yylval.ival = OPCODE_DIV   ; return COMMAND;}
EMT             { yylval.ival = OPCODE_EMT   ; return COMMAND;}
HALT            { yylval.ival = OPCODE_HALT  ; return COMMAND;}
INC             { yylval.ival = OPCODE_INC   ; return COMMAND;}
INCB            { yylval.ival = OPCODE_INCB  ; return COMMAND;}
IOT             { yylval.ival = OPCODE_IOT   ; return COMMAND;}
JMP             { yylval.ival = OPCODE_JMP   ; return COMMAND;}
JSR             { yylval.ival = OPCODE_JSR   ; return COMMAND;}
MOV             { yylval.ival = OPCODE_MOV   ; return COMMAND;}
MOVB            { yylval.ival = OPCODE_MOVB  ; return COMMAND;}
MUL             { yylval.ival = OPCODE_MUL   ; return COMMAND;}
NEG             { yylval.ival = OPCODE_NEG   ; return COMMAND;}
NEGB            { yylval.ival = OPCODE_NEGB  ; return COMMAND;}
NOP             { yylval.ival = OPCODE_NOP   ; return COMMAND;}
RETURN          { yylval.ival = OPCODE_RETURN; return COMMAND;}
SUB             { yylval.ival = OPCODE_SUB   ; return COMMAND;}
XOR             { yylval.ival = OPCODE_XOR   ; return COMMAND;}

[a-zA-Z][[a-zA-Z0-9]*   { yylval.sval = strdup(yytext); return STRING;}
^[a-zA-Z][[a-zA-Z0-9]*: {  yytext[strlen(yytext)-1] = '\0'; yylval.sval = strdup(yytext); return LABEL;}


.               { 
    char errMsg[128] = {'l','o','l'};
    snprintf(errMsg, 128, "lexer error: unknown lexem `%s`", yytext);    
    
    yyerror(nullptr, errMsg);
}
%%
