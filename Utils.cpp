#include "Utils.h"

namespace AST
{
    InstructionGroup GetInstructionGroup(const int opcode)
    {
        if (SingleOperandOpcodes.find(opcode) != SingleOperandOpcodes.end())
            return InstructionGroup::SingleOperand;

        if (DoubleOperandOpcodes.find(opcode) != DoubleOperandOpcodes.end())
            return InstructionGroup::DoubleOperand;

        if (OneAndHalfOpcodes.find(opcode) != OneAndHalfOpcodes.end())
            return InstructionGroup::OneAndHalf;

        if (BranchOpcodes.find(opcode) != BranchOpcodes.end())
            return InstructionGroup::Branch;

        return InstructionGroup::Unknown;
    }
}