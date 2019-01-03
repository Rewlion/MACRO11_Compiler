#include "CodeGenerator.h"
#include "Utils.h"
#include "ErrorHandling.h"
#include <iostream>
#include <assert.h>

namespace AST
{
    namespace
    {
        class FirstPass : public AstVisitor
        {
        public:
            FirstPass();
            ~FirstPass();

            virtual void Visit(CommandNode* node) override;
            virtual void Visit(OneOperandCommandNode* node) override;
            virtual void Visit(DoubleOperandCommandNode* node) override;

            ProgramNode* GetProgram();
            inline const std::map<std::string, int>& GetLabelsTable() const;

        private:
            void AddInstructionLabels(const CommandNode* node, const int instructionNumber);
            void AddLabel(const char* name, const int instructionNumber);
            inline void AddAdditionalInstructionWords();
            Word GetOperand(const OperandNode* node);

        private:
            ProgramNode *              Program;
            std::vector<CommandNode*>  Commands;
            std::map<std::string, int> LabelsTable;
            std::queue<Word>           AdditionalInstructionWords;
        };

        ProgramNode* FirstPass::GetProgram()
        {
            if (Commands.empty() == false)
            {
                CommandNode* n = Commands[0];
                for (size_t i = 1; i < Commands.size(); ++i)
                {
                    n->Next = Commands[i];
                    n = n->Next;
                }

                Program->Commands = Commands[0];
            }

            return Program;
        }

        const std::map<std::string, int>& FirstPass::GetLabelsTable() const
        {
            return LabelsTable;
        }

        void FirstPass::AddAdditionalInstructionWords()
        {
            while (AdditionalInstructionWords.empty() == false)
            {
                const Word i = AdditionalInstructionWords.front();
                Commands.push_back(new GeneratedInstructionNode(i, 0, -1)); //todo add info
                    AdditionalInstructionWords.pop();
            }
        }

        FirstPass::FirstPass()
            : Program(new ProgramNode(nullptr))
        {
        }

        FirstPass::~FirstPass()
        {
            delete Program;
        }

        void FirstPass::AddInstructionLabels(const CommandNode* node, const int instructionNumber)
        {
            for (LabelNode* l = node->Labels; l != nullptr; l = l->Next)
                AddLabel(l->Name, instructionNumber);
        }

        void FirstPass::AddLabel(const char* name, const int instructionNumber)
        {
            std::string l(name);
            LabelsTable[l] = instructionNumber;
        }

        void FirstPass::Visit(CommandNode* node)
        {
            const int instructionNumber = Commands.size() + 1;
            auto* i = new GeneratedInstructionNode(node->Opcode, node->Opcode, node->Line);
            i->SetInstructionNumber(instructionNumber);

            Commands.push_back(i);
            AddInstructionLabels(node, instructionNumber);
        }

        void FirstPass::Visit(OneOperandCommandNode* node)
        {
            Word raw = 0;
            raw |= node->Opcode;

            const OperandNode* first = node->First;
            const InstructionGroup g = GetInstructionGroup(node->Opcode);

            if (g == InstructionGroup::SingleOperand)
            {
                const Word op = GetOperand(first);
                raw |= op;
            }
            else if (g == InstructionGroup::Branch)
            {
                const int instructionNumber = Commands.size() + 1;
                if (first->AddrType == AddressingType::Label)
                {
                    auto it = LabelsTable.find(first->LabelName);
                    if (it != LabelsTable.end())
                    {
                        raw |= static_cast<uint8_t>((it->second - instructionNumber) * 2);
                    }
                    else
                    {
                        auto* i = new OneOperandCommandNode(*node);
                        i->SetInstructionNumber(instructionNumber);

                        Commands.push_back(i);
                        return;
                    }
                }
                else
                    raw |= first->Value;
            }

            const int instructionNumber = Commands.size() + 1;
            auto* i = new GeneratedInstructionNode(raw, node->Opcode, node->Line);
            i->SetInstructionNumber(instructionNumber);

            Commands.push_back(i);
            AddInstructionLabels(node, instructionNumber);
            AddAdditionalInstructionWords();
        }

        void FirstPass::Visit(DoubleOperandCommandNode* node)
        {
            Word raw = 0;
            raw |= node->Opcode;

            const OperandNode* first = node->First;
            const OperandNode* second = node->Second;
            const InstructionGroup g = GetInstructionGroup(node->Opcode);

            const Word firstOp = g == InstructionGroup::OneAndHalf ? GetOperand(first) & 07 : GetOperand(first);
            const Word secondOp = GetOperand(second);

            raw |= firstOp;
            raw |= (secondOp << 6);

            const int instructionNumber = Commands.size() + 1;
            auto* i = new GeneratedInstructionNode(raw, node->Opcode, node->Line);
            i->SetInstructionNumber(instructionNumber);

            Commands.push_back(i);
            AddInstructionLabels(node, instructionNumber);
            AddAdditionalInstructionWords();
        }

        Word FirstPass::GetOperand(const OperandNode* node)
        {
            Word op = 0;

            if (node->OpType == OperandType::Number)
            {
                op |= RegisterNumber::PC;
                AdditionalInstructionWords.push(node->Value);
            }
            else
            {
                op |= node->Value;
                if (node->AddrType == AddressingType::Index || node->AddrType == AddressingType::IndexDeferred)
                    AdditionalInstructionWords.push(node->IndexedOffset);
            }

            op |= (static_cast<int>(node->AddrType) << 3);

            return op;
        }

        class SecondPass : public AstVisitor
        {
        public:
            SecondPass(const std::map<std::string, int>& labelsTable, std::vector<Error>& errors);

            virtual void Visit(GeneratedInstructionNode* node) override;
            virtual void Visit(OneOperandCommandNode* node) override;

            inline const std::vector<Word>&& GetProgram() const;
            inline const std::vector<Error>& GetErrors() const;

        private:
            const std::map<std::string, int>& LabelsTable;
            std::vector<Word> Program;
            std::vector<Error>& Errors;
        };

        const std::vector<Word>&& SecondPass::GetProgram() const
        {
            return std::move(Program);
        }

        const std::vector<Error>& SecondPass::GetErrors() const
        {
            return Errors;
        }

        SecondPass::SecondPass(const std::map<std::string, int>& labelsTable, std::vector<Error>& errors)
            : LabelsTable(labelsTable)
            , Errors(errors)
        {
        }

        void SecondPass::Visit(GeneratedInstructionNode* node)
        {
            Program.push_back(node->Instruction);
        }

        void SecondPass::Visit(OneOperandCommandNode* node)
        {
            Word raw = 0;
            raw |= node->Opcode;

            const OperandNode* first = node->First;

            assert(GetInstructionGroup(node->Opcode) == InstructionGroup::Branch);
            assert(first->AddrType == AddressingType::Label);
            
            auto it = LabelsTable.find(first->LabelName);
            if (it != LabelsTable.end())
            {
                raw |= static_cast<uint8_t>((it->second - node->InstructionNumber) * 2);
                Program.push_back(raw);
            }
            else
            {
                Errors.push_back(Error{ node, std::string("Label doesn't exist:") + std::string(first->LabelName) });
            }
        }
    }

    std::vector<Word> CodeGenerator::Generate(AST::AbstractSyntaxTree* ast)
    {
        FirstPass fp;
        ast->Accept(&fp);

        ProgramNode* pn = fp.GetProgram();
        SecondPass sp{ fp.GetLabelsTable(), Errors };
        
        pn->Accept(&sp);
        std::vector<Word> program = std::move(sp.GetProgram());

        return program;
    }
}
