#include "SemanticAnalyzer.h"
#include "Utils.h"

#include <iostream>

namespace AST
{
    void SemanticAnalyzer::Visit(CommandNode* node)
    {
    
    }

    void SemanticAnalyzer::Visit(OneOperandCommandNode* node)
    {
        const InstructionGroup g = GetInstructionGroup(node->Opcode);

        switch (g)
        {
        case InstructionGroup::SingleOperand:
            return;

        case InstructionGroup::Branch:
            CheckBranchCommand(node);
            return;

        default:
            Errors.push_back(Error{ node, "wrong operands number." });
        }
    }

    void SemanticAnalyzer::Visit(DoubleOperandCommandNode* node)
    {
        const InstructionGroup g = GetInstructionGroup(node->Opcode);
        switch (g)
        {
        case InstructionGroup::DoubleOperand:
            return;

        case InstructionGroup::OneAndHalf:
            CheckOneAndHalfCommand(node);
            return;

        default:
            Errors.push_back(Error{node, "wrong operands number."});
        }
    }

    void SemanticAnalyzer::CheckOneAndHalfCommand(const DoubleOperandCommandNode* node)
    {
        const OperandNode* second = node->Second;
        if (second->OpType != OperandType::Register)
            Errors.push_back(Error{ node, "wrong operand (second operand is expected to be a register)." });
    }

    void SemanticAnalyzer::CheckBranchCommand(const OneOperandCommandNode* node)
    {
        const OperandNode* first = node->First;
        if (first->OpType != OperandType::Number && first->OpType != OperandType::LabelName)
            Errors.push_back(Error{ node, "wrong operand(label or int is expected)." });
    }
}