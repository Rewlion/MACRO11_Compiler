#pragma once

#include "Ast.h"
#include "ErrorHandling.h"

#include <vector>

namespace AST
{
    class SemanticAnalyzer : public AstVisitor
    {
    public:
        virtual void Visit(CommandNode* node) override;

        virtual void Visit(OneOperandCommandNode* node) override;

        virtual void Visit(DoubleOperandCommandNode* node) override;

        inline const std::vector<Error>& GetErrors() const;

    private:
        void CheckOneAndHalfCommand(const DoubleOperandCommandNode* node);
        void CheckBranchCommand(const OneOperandCommandNode* node);

    private:
        std::vector<Error> Errors;
    };

    const std::vector<Error>& SemanticAnalyzer::GetErrors() const
    {
        return Errors;
    }
}