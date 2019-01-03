#pragma once

#include "Ast.h"
#include "ErrorHandling.h"

#include <vector>
#include <queue>
#include <string>

namespace AST
{
    class CodeGenerator
    {
    public:
        std::vector<Word> Generate(AST::AbstractSyntaxTree* ast);
       
        inline const std::vector<Error>& GetErrors() const;

    private:
        std::vector<Error> Errors;
    };

    const std::vector<Error>& CodeGenerator::GetErrors() const
    {
        return Errors;
    }
}