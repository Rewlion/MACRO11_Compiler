#include "Compiler.h"
#include "SemanticAnalyzer.h"
#include "ErrorHandling.h"
#include "CodeGenerator.h"

#include "optparse.h"

#include <fstream>
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

std::vector<char> Compiler::ReadDataFile(const std::string& path)
{
    std::ifstream f{ path, std::ios::binary | std::ios::ate };
    
    if (f.is_open())
    {
        const size_t size = f.tellg();
        if (size != 0)
        {
            f.seekg(std::ios::beg);

            std::vector<char> data;
            data.resize(size);

            f.read(data.data(), size);

            return data;
        }
        else
            std::cerr << "Can't read a data file: file is empty.\n";
    }
    else
        std::cerr << "Can't read the data file: can't open the data file.\n";

    exit(-1);
}

void Compiler::Compile(int argc, char** argv)
{
    optparse::OptionParser parser = optparse::OptionParser().description("MACRO11 compiler");

    parser.add_option("-i").help("input file.").dest("input");
    parser.add_option("-o").help("output file.").dest("out");
    parser.add_option("-d").help("data file.").dest("data");

    const optparse::Values options = parser.parse_args(argc, argv);
    if (options.is_set("input") == false || options.is_set("out") == false)
    {
        parser.print_help();
        exit(-1);
    }

    uint64_t dataSize = 0;
    std::vector<char> data;
    if (options.is_set("data"))
    {
        data = ReadDataFile(options["data"]);
        dataSize = data.size();
    }

    const std::string& sourceFile = options["input"];
    AST::AbstractSyntaxTree ast = Parse(sourceFile.c_str());
    
    AST::SemanticAnalyzer sa;
    ast.Accept(&sa);
    DumpErrors(sa.GetErrors());

    AST::CodeGenerator codeGen;
    const std::vector<Word>& program = codeGen.Generate(&ast);
    DumpErrors(codeGen.GetErrors());
    
    FILE* f = fopen("dump.txt", "w");
    for (const Word instruction : program)
    {
        std::fprintf(f, "%07o\n", instruction);
    }
    fclose(f);

    uint64_t programSize = program.size() * sizeof(Word);

    const std::string& outputFile = options["out"];
    f = fopen(outputFile.c_str(), "w");
    fwrite(&dataSize, sizeof(uint64_t), 1, f);
    fwrite(data.data(), sizeof(Byte), dataSize, f);
    fwrite(program.data(), sizeof(Byte), programSize, f);

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
    Compiler c;
    c.Compile(argc, argv);
}