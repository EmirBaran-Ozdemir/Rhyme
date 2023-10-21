#include "rhypch.h"
#include "Generator.h"


namespace Compiler {

	Generator::Generator(const Node::Program& program)
		: m_Program(program)
	{
	}

	void Generator::GenerateExpression(const Node::Expr& expr)
	{
		struct ExpressionVisitor {
			Generator* gen;
			void operator()(const Node::ExprIntLit& exprIntLit)
			{
				gen->Move("rax", exprIntLit.int_lit.value.value());
				gen->Push("rax");
			}
			void operator()(const Node::ExprIdent& exprIdent)
			{
				if (!gen->m_VariableList.contains(exprIdent.ident.value.value()))
				{
					gen->ThrowErrorEx("Identifier is not declared", exprIdent.ident.value.value());
				}
				const auto& var = gen->m_VariableList.at(exprIdent.ident.value.value());
				std::stringstream offset;
				offset << "QWORD [rsp + " << (gen->m_StackSize - var.stackLocation - 1) * 8 << "]";
				gen->Push(offset.str());
			}
		};
		ExpressionVisitor visitor{.gen = this};
		std::visit(visitor, expr.var);
	}

	void Generator::GenerateStatement(const Node::Statement& statement) 
	{
		struct StatementVisitor {
			Generator* gen;
			void operator()(const Node::StatementExit& statementExit) 
			{
				gen->GenerateExpression(statementExit.expr);
				gen->Move("rax", "60");
				gen->Pop("rdi");
				gen->m_Output << "    syscall\n";
			}
			void operator()(const Node::StatementVar& statementVar)
			{
				if (gen->m_VariableList.contains(statementVar.ident.value.value()))
				{
					gen->ThrowErrorEx("Identifier already declared", statementVar.ident.value.value());
				}
				gen->m_VariableList.insert({ statementVar.ident.value.value(), Variable {.stackLocation = gen->m_StackSize} });
				gen->GenerateExpression(statementVar.expr);
			}
			
		};

		StatementVisitor visitor{.gen = this};
		std::visit(visitor, statement.var);
	}

	std::string Generator::GenerateProgram() 
	{
			m_Output << "global _start\n_start:\n";
			for (const Node::Statement& statement : m_Program.statement)
				GenerateStatement(statement);

			Move("rax", "60");
			Move("rdi", "0");
			m_Output << "    syscall\n";
			return m_Output.str();

	}

	void Generator::Push(const std::string& reg)
	{
		m_Output << "	push " << reg << "\n";
		m_StackSize++;
	}

	void Generator::Pop(const std::string& reg)
	{
		m_Output << "	pop " << reg << "\n";
		m_StackSize--;
	}

	void Generator::Move(const std::string& reg, const std::string& value)
	{
		m_Output << "	mov " << reg << ", " << value << "\n";
	}

	void Generator::ThrowError(const std::string& message)
	{
		throw std::invalid_argument("ERROR::PARSER::" + message);
	}
	void Generator::ThrowErrorEx(const std::string& message, const std::string& ident)
	{
		throw std::invalid_argument("ERROR::PARSER::" + message + ": " + ident);
	}
}
