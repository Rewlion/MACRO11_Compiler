#pragma once

#include "Ast.h"

#include <string>
#include <vector>

namespace AST
{
    struct Error
    {
        const CommandNode* ErrorNode;
        std::string Message;
    };

    class ErrorDumper
    {
    public:
        void Dump(const std::vector<Error>& errors);
    };
}