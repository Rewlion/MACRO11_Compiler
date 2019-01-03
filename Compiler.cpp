#include "Compiler.h"
#include "SemanticAnalyzer.h"
#include "ErrorHandling.h"
#include "CodeGenerator.h"

#include <cstdio>
#include <cstdlib>

extern int yylex();
extern int yyparse(AST::AbstractSyntaxTree* ast);
extern FILE *yyin;
extern int yylineno;
extern int yydebug;

namespace
{
    void DumpErrors(const std::vector<AST::Error>& errors)
    {
        if (errors.empty() == false)
        {
            AST::ErrorDumper().Dump(errors);
            exit(-1);
        }
    }
}

void Compiler::Compile(const char* sourceFile, const char* outputFile)
{
    AST::AbstractSyntaxTree ast = Parse(sourceFile);
    
    AST::SemanticAnalyzer sa;
    ast.Accept(&sa);
    DumpErrors(sa.GetErrors());

    AST::CodeGenerator codeGen;
    const std::vector<Word>& program = codeGen.Generate(&ast);
    DumpErrors(codeGen.GetErrors());
    
    FILE* f = fopen(outputFile, "w");

    for (const Word instruction : program)
    {
        std::fprintf(f, "%07o\n", instruction);
    }
    
    fclose(f);
}

AST::AbstractSyntaxTree Compiler::Parse(const char* sourceFile) const
{
    FILE* f = fopen(sourceFile, "r");

    if (!f)
    {
        fprintf(stderr, "can.t open a file %s", sourceFile);
        exit(-1);
    }

    yyin = f;

    AST::AbstractSyntaxTree ast;
    yyparse(&ast);

    fclose(f);
    return ast;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "input: {source} {output}");
        exit(-1);
    }
    
    //yydebug = 1;

    Compiler c;
    c.Compile(argv[1], argv[2]);
}