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
            unsigned int GetOperandSize(const OperandNode* node);

        private:
            ProgramNode *              Program;
            unsigned int               CurrentProgramSize;
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


        FirstPass::FirstPass()
            : Program(new ProgramNode(nullptr))
            , CurrentProgramSize(0)
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
            const int instructionNumber = CurrentProgramSize++;
            
            AddInstructionLabels(node, instructionNumber);
            
            Commands.push_back(node);
        }

        void FirstPass::Visit(OneOperandCommandNode* node)
        {
            const int instructionNumber = CurrentProgramSize;

            CurrentProgramSize += 1 + GetOperandSize(node->First);

            Commands.push_back(node);
            AddInstructionLabels(node, instructionNumber);
        }

        void FirstPass::Visit(DoubleOperandCommandNode* node)
        {
            const unsigned int instructionNumber = CurrentProgramSize;

            CurrentProgramSize += 1 + GetOperandSize(node->First) + GetOperandSize(node->Second);

            Commands.push_back(node);
            AddInstructionLabels(node, instructionNumber);
        }

        unsigned int FirstPass::GetOperandSize(const OperandNode* node)
        {
            unsigned int size = 0;

            if (   node->OpType   == OperandType::Number
                || node->AddrType == AddressingType::Index
                || node->AddrType == AddressingType::IndexDeferred)
            {
                size = 1;
            }
            else
            {
                size = 0;
            }

            return size;
        }

        class SecondPass : public AstVisitor
        {
        public:
            SecondPass(const std::map<std::string, int>& labelsTable, std::vector<Error>& errors);

            virtual void Visit(CommandNode* node) override;
            virtual void Visit(OneOperandCommandNode* node) override;
            virtual void Visit(DoubleOperandCommandNode* node) override;

            inline const std::vector<Word>&& GetProgram() const;
            inline const std::vector<Error>& GetErrors() const;

        private:
            Word GetRawOperand(const OperandNode* node, std::vector<Word>* additionalWords) const;
            Word GetRawLabel(const std::string& labelName, CommandNode* node) const;
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

        Word SecondPass::GetRawOperand(const OperandNode* node, std::vector<Word>* additionalWords) const
        {
            Word op = 0;

            if (node->OpType == OperandType::Number)
            {
                additionalWords->push_back(node->Value);
                op = RegisterNumber::PC;
            }
            else
            {
                if (node->AddrType == AddressingType::Index || node->AddrType == AddressingType::IndexDeferred)
                    additionalWords->push_back(node->IndexedOffset);

                op = node->Value;
            }

            op |= (static_cast<int>(node->AddrType) << 3);
            return op;
        }

        Word SecondPass::GetRawLabel(const std::string& labelName, CommandNode* node) const
        {
            auto it = LabelsTable.find(labelName);
            if (it != LabelsTable.end())
            {
                return static_cast<uint8_t>(it->second);
            }
            else
            {
                Errors.push_back(Error{ node, std::string("Label doesn't exist:") + labelName });
                return 0;
            }
        }

        void SecondPass::Visit(CommandNode* node)
        {
            Program.push_back(node->Opcode);
        }

        void SecondPass::Visit(OneOperandCommandNode* node)
        {
            std::vector<Word> additionalWords;
            const unsigned int instructionNumber = Program.size();
            Word raw = node->Opcode;

            const OperandNode* first = node->First;
            if (first->AddrType == AddressingType::Label)
            {
                const Word rawLabel = GetRawLabel(first->LabelName, node);
                
                if (GetInstructionGroup(node->Opcode) == InstructionGroup::Branch)
                {
                    const Byte offset = static_cast<Byte>(rawLabel - instructionNumber);
                    raw |= offset;
                }
                else
                {
                    Word op = RegisterNumber::PC;
                    op |= (static_cast<int>(AddressingType::AutoIncrement) << 3);
                    raw |= op;
                    additionalWords.push_back(rawLabel + GetROMBegining());
                }
            }
            else
            {
                const Word op = GetRawOperand(first, &additionalWords);
                raw |= op;
            }
            
            Program.push_back(raw);
            if(additionalWords.empty() == false)
                Program.push_back(additionalWords[0]);
        }
    }

    void SecondPass::Visit(DoubleOperandCommandNode* node)
    {
        std::vector<Word> additionalWords;
        const unsigned int instructionNumber = Program.size();
        Word raw = node->Opcode;

        const OperandNode* first = node->First;
        const OperandNode* second = node->Second;
        assert((first->AddrType != AddressingType::Label) && (second->AddrType != AddressingType::Label));

        const InstructionGroup g = GetInstructionGroup(node->Opcode);

        const Word firstOp = g == InstructionGroup::OneAndHalf ? GetRawOperand(first, &additionalWords) & 07 : GetRawOperand(first, &additionalWords);
        const Word secondOp = GetRawOperand(second, &additionalWords);

        raw |= secondOp;
        raw |= (firstOp << 6);

        Program.push_back(raw);
        for (const Word w : additionalWords)
            Program.push_back(w);
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
