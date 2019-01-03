#pragma once

#include <cstdint>
#include <map>
#include <string>

typedef uint16_t Word;

enum RegisterNumber
{
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4, 
    R5 = 5,
    R6 = 6, SP = 6,
    R7 = 7, PC = 7,
};

enum class OperandType : unsigned char
{
    Register  = 0,
    Number    = 1,
    LabelName = 2,
};

enum class AddressingType : unsigned char
{
    Register = 0,
    RegisterDeferred = 1,
    AutoIncrement = 2,
    AutoIncrementDeferred = 3,
    AutoDecrement = 4,
    AutoDecrementDeferred = 5,
    Index = 6,
    IndexDeferred = 7,
    Label,
};

enum class InstructionGroup : unsigned char
{
    Unknown       = 0,
    SingleOperand = 1,
    DoubleOperand = 2,
    OneAndHalf    = 3,
    Branch        = 4,
    Condition     = 5,
    Other         = 6
};

static std::map<Word, std::string> SingleOperandOpcodes = {
    { 0005000, "CLR"  },
    { 0105000, "CLRB" },
    { 0005100, "COM"  },
    { 0105100, "COMB" },
    { 0005200, "INC"  },
    { 0105200, "INCB" },
    { 0005300, "DEC"  },
    { 0105300, "DECB" },
    { 0005400, "NEG"  },
    { 0105400, "NEGB" },
    { 0005700, "TST"  },
    { 0105700, "TSTB" },
    { 0006200, "ASR"  },
    { 0106200, "ASRB" },
    { 0006300, "ASL"  },
    { 0106300, "ASLB" },
    { 0006000, "ROR"  },
    { 0106000, "RORB" },
    { 0006100, "ROL"  },
    { 0106100, "ROLB" },
    { 0000300, "SWAB" },
    { 0005500, "ADC"  },
    { 0105500, "ADCB" },
    { 0005600, "SBC"  },
    { 0105600, "SBCB" },
    { 0006700, "SXT"  },
};

static std::map<Word, std::string> DoubleOperandOpcodes = {
    { 0010000, "MOV"  },
    { 0110000, "MOVB" },
    { 0020000, "CMP"  },
    { 0120000, "CMPB" },
    { 0060000, "ADD"  },
    { 0160000, "SUB"  },
    { 0030000, "BIT"  },
    { 0130000, "BITB" },
    { 0040000, "BIC"  },
    { 0140000, "BICB" },
    { 0050000, "BIS"  },
    { 0150000, "BISB" },
};

static std::map<Word, std::string> OneAndHalfOpcodes = {
    { 0070000, "MUL" },
    { 0071000, "DIV" },
    { 0072000, "ASH" },
    { 0073000, "ASHC"},
    { 0074000, "XOR" },
};

static std::map<Word, std::string> BranchOpcodes = {
    { 0000400, "BR"  },
    { 0001000, "BNE" },
    { 0001400, "BEQ" },
    { 0100000, "BPL" },
    { 0100400, "BMI" },
    { 0102000, "BVC" },
    { 0102400, "BVS" },
    { 0103000, "BCC" },
    { 0103400, "BCS" },
    { 0002000, "BGE" },
    { 0002400, "BLT" },
    { 0003000, "BGT" },
    { 0003400, "BLE" },
    { 0101000, "BHI" },
    { 0101400, "BLOS"},
    { 0103000, "BHIS"},
    { 0103400, "BLO" },
};

enum OpCodes
{
   OPCODE_ADC  = 0005500,
   OPCODE_ADCB = 0105500,
   OPCODE_ADD  = 0060000,
   OPCODE_ASH  = 0072000,
   OPCODE_ASHC = 0073000,
   OPCODE_ASL  = 0006300,
   OPCODE_ASLB = 1006300,
   OPCODE_ASR  = 0006200,
   OPCODE_ASRB = 1006200,
   OPCODE_BCC  = 0103000,
   OPCODE_BCS  = 0103400,
   OPCODE_BEQ  = 0001400,
   OPCODE_BGE  = 0002000,
   OPCODE_BGT  = 0003000,
   OPCODE_BHI  = 0101000,
   OPCODE_BHIS = 0103000,
   OPCODE_BIC  = 0040000,
   OPCODE_BICB = 0140000,
   OPCODE_BIS  = 0050000,
   OPCODE_BISB = 0150000,
   OPCODE_BIT  = 0030000,
   OPCODE_BITB = 0130000,
   OPCODE_BLE  = 0003400,
   OPCODE_BLO  = 0103400,
   OPCODE_BLOS = 0101400,
   OPCODE_BLT  = 0002400,
   OPCODE_BMI  = 0100400,
   OPCODE_BNE  = 0001000,
   OPCODE_BPL  = 0100000,
   OPCODE_BPT  = 0000003,
   OPCODE_BR   = 0000400,
   OPCODE_BVC  = 0102000,
   OPCODE_BVS  = 0102400,
   OPCODE_CALL = 0004700,
   OPCODE_CALLR = 0000100,
   OPCODE_CCC  = 0000257,
   OPCODE_CLC  = 0000241,
   OPCODE_CLN  = 0000250,
   OPCODE_CLR  = 0005000,
   OPCODE_CLRB = 0105000,
   OPCODE_CLV  = 0000242,
   OPCODE_CLZ  = 0000244,
   OPCODE_CMP  = 0020000,
   OPCODE_CMPB = 0120000,
   OPCODE_COM  = 0005100,
   OPCODE_COMB = 0105100,
   OPCODE_DEC  = 0005300,
   OPCODE_DECB = 0105300,
   OPCODE_DIV  = 0071000,
   OPCODE_EMT  = 0104000,
   OPCODE_HALT = 0000000,
   OPCODE_INC  = 0005200,
   OPCODE_INCB = 0105200,
   OPCODE_IOT  = 0000004,
   OPCODE_JMP  = 0000100,
   OPCODE_JSR  = 0004000,
   OPCODE_MOV  = 0010000,
   OPCODE_MOVB = 0110000,
   OPCODE_MUL  = 0070000,
   OPCODE_NEG  = 0005400,
   OPCODE_NEGB = 0105400,
   OPCODE_NOP  = 0000240,
   OPCODE_RETURN = 0000207,
   OPCODE_SUB  = 0160000,
   OPCODE_XOR  = 0074000,
};
