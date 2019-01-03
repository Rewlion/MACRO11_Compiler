#pragma once

#include "Macro11Common.h"
#include <vector>

namespace AST
{
    class AstVisitor;

    class Node
    {
    public:
        virtual void Accept(AstVisitor* visitor);
        virtual ~Node() {}

    };

    class LabelNode : public Node
    {
    public:
        LabelNode(const char* name);
        virtual void Accept(AstVisitor* visitor);
        virtual ~LabelNode() override;

    public:
        const char* Name;
        LabelNode* Next;
    };

    class OperandNode : public Node
    {
    public:
        OperandNode(const OperandType optype, const int value, const AddressingType addrType, const char* labelName = nullptr, const int indexedOffset = 0);
        virtual void Accept(AstVisitor* visitor) override;
        
    public:
        OperandType    OpType;
        int            Value;
        AddressingType AddrType;
        int            IndexedOffset;
        const char*    LabelName;
    };

    class CommandNode : public Node
    {
    public:
        CommandNode(const int opcode, const int line);
        virtual ~CommandNode() override;
        virtual void Accept(AstVisitor* visitor) override;

        inline void SetInstructionNumber(const int number) const
        {
            InstructionNumber = number;
        }

    public:
        const int    Opcode;
        const int    Line;
        mutable int  InstructionNumber;
        CommandNode* Next;
        LabelNode*   Labels;
    };

    class GeneratedInstructionNode : public CommandNode
    {
    public:
        GeneratedInstructionNode(const Word instruction, const Word opcode, const int line);
        virtual void Accept(class AstVisitor* visitor) override;

    public:
        const Word Instruction;
    };

    class OneOperandCommandNode : public CommandNode
    {
    public:
        OneOperandCommandNode(const int opcode, OperandNode* first, const int line);
        virtual ~OneOperandCommandNode() override;
        virtual void Accept(AstVisitor* visitor) override;

    public:
        OperandNode* First;
    };

    class DoubleOperandCommandNode : public CommandNode
    {
    public:
        DoubleOperandCommandNode(const int opcide, OperandNode* first, OperandNode* second, const int line);
        virtual ~DoubleOperandCommandNode() override;
        virtual void Accept(AstVisitor* visitor) override;

    public:
        OperandNode* First;
        OperandNode* Second;
    };

    class ProgramNode : public Node
    {
    public:
        ProgramNode(CommandNode* commands);
        virtual ~ProgramNode() override;
        virtual void Accept(AstVisitor* visitor) override;

    public:
        CommandNode* Commands;
    };

    class AstVisitor
    {
    public:
        virtual void Visit(Node* node)                                    {}
        virtual void Visit(LabelNode* node)                               {}
        virtual void Visit(OperandNode* node)                             {}
        virtual void Visit(CommandNode* node)                             {}
        virtual void Visit(GeneratedInstructionNode* node)                {}
        virtual void Visit(OneOperandCommandNode* node)                   {}
        virtual void Visit(DoubleOperandCommandNode* node)                {}
        virtual void Visit(ProgramNode* node)                             {}
    };

    class AbstractSyntaxTree
    {
    public:
        AbstractSyntaxTree();

        void SetProgram(ProgramNode* node);
        void Accept(AstVisitor* visitor);

    private:
        ProgramNode* Program;
    };
}