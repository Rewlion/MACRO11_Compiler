#include "ErrorHandling.h"
#include <iostream>

namespace AST
{
    void ErrorDumper::Dump(const std::vector<Error>& errors)
    {
        for (const auto& e : errors)
        {
            std::fprintf(stderr, "line:%d error:%s\n", e.ErrorNode->Line, e.Message.c_str());
        }
    }
}