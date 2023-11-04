#include "rhypch.h"
#include "Generator.h"


namespace Compiler {

	Generator::Generator(const Node::Program& program)
		: m_Program(program)
	{
	}

	void Generator::GenerateTerm(const Node::Term* term)
	{
		struct TermVisitor {
			Generator* gen;
			void operator()(const Node::TermIntLit* termIntLit) const
			{
				gen->Move("rax", termIntLit->intLit.value.value());
				gen->Push("rax");
			}
			void operator()(const Node::TermIdent* termIdent) const
			{
				if (!gen->m_VariableList.contains(termIdent->ident.value.value()))
				{
					gen->ThrowError("Identifier is not declared", termIdent->ident.value.value());
				}
				const auto& var = gen->m_VariableList.at(termIdent->ident.value.value());
				std::stringstream offset;
				offset << "QWORD [rsp + " << (gen->m_StackSize - var.stackLocation - 1) * 8 << "]\n";
				gen->Push(offset.str());
			}
			void operator()(const Node::TermParenthesis* termParenthesis) const
			{
				gen->GenerateExpression(termParenthesis->expr);
			}

		};
		TermVisitor visitor({ .gen = this });
		std::visit(visitor, term->var);
	}

	void Generator::GenerateBinExpression(const Node::BinExpr* expr)
	{
		struct BinExprVisitor
		{
			Generator* gen;
			void operator()(const Node::BinExprAddition* add)
			{
				gen->GenerateExpression(add->rhs);
				gen->GenerateExpression(add->lhs);
				gen->Pop("rax");
				gen->Pop("rbx");
				gen->m_Output << "\tadd rax, rbx\n";
				gen->Push("rax");
			}
			void operator()(const Node::BinExprMultiplication* multi)
			{
				gen->GenerateExpression(multi->rhs);
				gen->GenerateExpression(multi->lhs);
				gen->Pop("rax");
				gen->Pop("rbx");
				gen->m_Output << "\tmul rbx\n";
				gen->Push("rax");
			}
			void operator()(const Node::BinExprSubtraction* subtr)
			{
				gen->GenerateExpression(subtr->rhs);
				gen->GenerateExpression(subtr->lhs);
				gen->Pop("rax");
				gen->Pop("rbx");
				gen->m_Output << "\tsub rax, rbx\n";
				gen->Push("rax");
			}
			void operator()(const Node::BinExprDivision* div)
			{
				gen->GenerateExpression(div->rhs);
				gen->GenerateExpression(div->lhs);
				gen->Pop("rax");
				gen->Pop("rbx");
				gen->m_Output << "\tidiv rbx\n";
				gen->Push("rax");
			}
			void operator()(const Node::BinExprLessThan* less)
			{
				gen->GenerateExpression(less->rhs);
				gen->GenerateExpression(less->lhs);
				gen->Pop("rax");
				gen->Pop("rbx");
				gen->m_Output << "\tcmp rax, rbx\n";
				if (gen->m_StatementIf)
				{
					gen->m_Output << "\tjl if" << gen->m_IfCount << "\n";
					gen->m_Output << "\tjmp end_if" << gen->m_IfCount << "\n";
				}
				else
				{
					gen->m_Output << "\tsetl al\n";
					gen->Push("rax");
				}
			}
			void operator()(const Node::BinExprGreaterThan* greater)
			{
				gen->GenerateExpression(greater->rhs);
				gen->GenerateExpression(greater->lhs);
				gen->Pop("rax");
				gen->Pop("rbx");
				gen->m_Output << "\tcmp rax, rbx\n";
				if (gen->m_StatementIf)
				{
					gen->m_Output << "\tjg if" << gen->m_IfCount << "\n";
					gen->m_Output << "\tjmp end_if" << gen->m_IfCount << "\n";
				}
				else
				{
					gen->m_Output << "\tsetg al\n";
					gen->Push("rax");
				}

			}
		};
		BinExprVisitor visitor({ .gen = this });
		std::visit(visitor, expr->binExprType);
	}
	void Generator::GenerateExpression(const Node::Expr* expr)
	{
		struct  ExpressionVisitor
		{
			Generator* gen;
			void operator()(const Node::Term* term) const
			{
				gen->GenerateTerm(term);
			}
			void operator()(const Node::BinExpr* binExpr) const
			{
				gen->GenerateBinExpression(binExpr);
			}
		};
		ExpressionVisitor visitor({ .gen = this });
		std::visit(visitor, expr->var);
	}


	void Generator::GenerateStatement(const Node::Statement* statement)
	{
		struct StatementVisitor {
			Generator* gen;
			void operator()(const Node::StatementExit* statementExit) const
			{
				gen->GenerateExpression(statementExit->expr);
				gen->Move("rax", "60");
				gen->Pop("rdi");
				gen->m_Output << "\tsyscall\n";
			}
			void operator()(const Node::StatementVar* statementVar) const
			{
				if (gen->m_VariableList.contains(statementVar->ident.value.value()))
				{
					gen->ThrowError("Identifier already declared", statementVar->ident.value.value());
				}
				gen->m_VariableList.insert({ statementVar->ident.value.value(), Variable {.stackLocation = gen->m_StackSize} });
				gen->GenerateExpression(statementVar->expr);
			}
			void operator()(const Node::StatementIf* statementIf) const
			{
				gen->m_IfCount++;
				gen->m_StatementIf = true;
				gen->GenerateExpression(statementIf->expr);
				gen->m_Output << "if" << gen->m_IfCount << ":\n";
				gen->GenerateStatement(statementIf->statement);
				gen->m_Output << "end_if" << gen->m_IfCount << ":\n";
				gen->m_StatementIf = false;
			}
		};

		StatementVisitor visitor({ .gen = this });
		std::visit(visitor, statement->var);
	}

	std::string Generator::GenerateProgram()
	{
		m_Output << "global _start\n_start:\n";
		for (const Node::Statement* statement : m_Program.statement)
			GenerateStatement(statement);

		Move("rax", "60");
		Move("rdi", "0");
		m_Output << "\tsyscall\n";
		return m_Output.str();
	}

	void Generator::Push(const std::string& reg)
	{
		m_Output << "\tpush " << reg << "\n";
		m_StackSize++;
	}

	void Generator::Pop(const std::string& reg)
	{
		m_Output << "\tpop " << reg << "\n";
		m_StackSize--;
	}

	void Generator::Move(const std::string& reg, const std::string& value)
	{
		m_Output << "\tmov " << reg << ", " << value << "\n";
	}

	void Generator::ThrowError(const std::string& message)
	{
		throw std::invalid_argument("ERROR::PARSER::" + message);
	}
	void Generator::ThrowError(const std::string& message, const std::string& ident)
	{
		throw std::invalid_argument("ERROR::PARSER::" + message + ": " + ident);
	}
}
