#pragma once
#include "Parser.h"

namespace Compiler {


	class Generator {
		struct Variable {
			std::string name;
			size_t stackLocation;
		};

	public:
		Generator(const Node::Program& program, bool enableDebugging = false);
		void GenerateExpression(const Node::Expr* expr);
		void GenerateBinExpression(const Node::BinExpr* expr);
		void GenerateTerm(const Node::Term* term);
		void GenerateStatement(const Node::Statement* statement);
		void GenerateScope(const Node::StatementScope* scope);
		std::string GenerateProgram();

		template <typename Operator>
		void GenerateArithmeticBinaryExpression(const Operator* op, const char* operation)
		{
			GenerateExpression(op->rhs);
			GenerateExpression(op->lhs);
			Pop("rax");
			Pop("rbx");
			m_Output << "\t" << operation << " rax, rbx\n";
			Push("rax");
		}
		template <typename Operator>
		void GenerateComparisonBinaryExpression(const Operator* op, const char* operation)
		{
			GenerateExpression(op->rhs);
			GenerateExpression(op->lhs);
			Pop("rax");
			Pop("rbx");
			m_Output << "\tcmp rax,rbx\n";
			if (m_StatementIf)
			{
				Push("rax");
				m_Output << "\t" << operation << " L" << m_IfCount << "\n";
			}
			else
			{
				m_Output << "\tsetl al\n";
				Push("rax");
			}
		}
	private:
		void Push(const std::string& reg);
		void Pop(const std::string& reg);
		void Move(const std::string& reg, const std::string& value);

		void BeginScope();
		void EndScope();

		void DebugMessage(const std::string& message);
	private:
		const Node::Program m_Program;
		std::stringstream m_Output;
		bool m_EnableDebug;

		size_t m_StackSize = 0;
		size_t m_IfCount = 0;
		size_t m_ElseCount = 0;
		size_t m_EndIf = 0;
		bool m_StatementIf;
		std::vector<Variable> m_VariableList{};
		std::vector<size_t> m_Scopes{};


	};
}