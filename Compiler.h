#pragma once

#include "Ast.h"

class Compiler
{
public:
    void Compile(const char* sourceFile, const char* outputFile);

private:
    AST::AbstractSyntaxTree Parse(const char* sourceFile) const;
};