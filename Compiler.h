#pragma once

#include "Ast.h"

class Compiler
{
public:
    void Compile(int argc, char** argv);

private:
    std::vector<char> ReadDataFile(const std::string& path);

private:

    AST::AbstractSyntaxTree Parse(const char* sourceFile) const;
};