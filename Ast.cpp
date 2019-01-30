#include "Ast.h"

namespace AST
{

    void Node::Accept(AstVisitor* visitor)
    {
        visitor->Visit(this);
    }

    LabelNode::LabelNode(const char* name)
        : Name(name)
        , Next(nullptr)
    {
    }

    LabelNode::~LabelNode()
    {
        delete Next;
        free(const_cast<char*>(Name));
    }

    void LabelNode::Accept(AstVisitor* visitor)
    {
        visitor->Visit(this);
    }

    OperandNode::OperandNode(const OperandType optype, const int value, const AddressingType addrType, const char* labelName, const int indexedOffset)
        : OpType(optype)
        , Value(value)
        , AddrType(addrType)
        , IndexedOffset(indexedOffset)
        , LabelName(labelName)
    {
    }

    void OperandNode::Accept(AstVisitor* visitor)
    {
        visitor->Visit(this);
    }

    CommandNode::CommandNode(const int opcode, const int line)
        : Opcode(opcode)
        , Line(line)
        , Next(nullptr)
        , Labels(nullptr)
    {
    }

    CommandNode::~CommandNode()
    {
        delete(Next);
    }

    void CommandNode::Accept(AstVisitor* visitor)
    {
        visitor->Visit(this);
    }

    OneOperandCommandNode::OneOperandCommandNode(const int opcode, OperandNode* first, const int line)
        : CommandNode(opcode, line)
        , First(first)
    {
    }

    OneOperandCommandNode::~OneOperandCommandNode()
    {
        delete First;
    }

    void OneOperandCommandNode::Accept(AstVisitor* visitor)
    {
        visitor->Visit(this);
    }

    DoubleOperandCommandNode::DoubleOperandCommandNode(const int opcode, OperandNode* first, OperandNode* second, const int line)
        : CommandNode(opcode, line)
        , First(first)
        , Second(second)
    {
    }

    DoubleOperandCommandNode::~DoubleOperandCommandNode()
    {
        delete First;
        delete Second;
    }
    
    void DoubleOperandCommandNode::Accept(AstVisitor* visitor)
    {
        visitor->Visit(this);
    }

    void ProgramNode::Accept(AstVisitor* visitor)
    {
        visitor->Visit(this);
        
        for (CommandNode* n = Commands; n != nullptr; n = n->Next)
            n->Accept(visitor);
    }

    ProgramNode::ProgramNode(CommandNode* commands)
        : Commands(commands)
    {
    }

    ProgramNode::~ProgramNode()
    {
        delete Commands;
    }

    AbstractSyntaxTree::AbstractSyntaxTree()
        : Program(nullptr)
    {
    }

    void AbstractSyntaxTree::SetProgram(ProgramNode* node)
    {
        if (Program)
            delete Program;

        Program = node;
    }

    void AbstractSyntaxTree::Accept(AstVisitor* visitor)
    {
        Program->Accept(visitor);
    }
}