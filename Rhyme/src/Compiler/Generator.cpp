#include "rhypch.h"
#include "Generator.h"

namespace Compiler {

	Generator::Generator(const Node::Program& program, bool enableDebug)
		: m_Program(program), m_EnableDebug(enableDebug)
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
				auto it = std::find_if(gen->m_VariableList.cbegin(), gen->m_VariableList.cend(), [&](const Variable& var) {
					return var.name == termIdent->ident.value.value();
					});
				if (it == gen->m_VariableList.cend())
				{
					THROW_ERROR_ARGS("Identifier is not declared", termIdent->ident.value.value());
				}

				std::stringstream offset;
				if (gen->m_EnableDebug)
					offset << "QWORD [rsp + " << (gen->m_StackSize - (*it).stackLocation - 1) * 8 << "] ;" << termIdent->ident.value.value();
				else
					offset << "QWORD [rsp + " << (gen->m_StackSize - (*it).stackLocation - 1) * 8 << "]";
				gen->Push(offset.str());
			}
			void operator()(const Node::TermParenthesis* termParenthesis) const
			{
				gen->GenerateExpression(termParenthesis->expr);
			}
			void operator()(const Node::TermUnary* termUnary) const
			{
				//gen->GenerateExpression(termUnary->expr);
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
			const Node::BinExpr* expr;

			void operator()(const Node::BinExprAddition* add) const
			{
				gen->DebugMessage("Addition");
				gen->GenerateArithmeticBinaryExpression(add, "add");
			}

			void operator()(const Node::BinExprMultiplication* multi) const
			{
				gen->DebugMessage("Multiplication");
				gen->GenerateArithmeticBinaryExpression(multi, "imul");
			}

			void operator()(const Node::BinExprSubtraction* subtr) const
			{
				gen->DebugMessage("Subtraction");
				gen->GenerateArithmeticBinaryExpression(subtr, "sub");
			}

			void operator()(const Node::BinExprDivision* div) const
			{
				gen->DebugMessage("Division");
				gen->GenerateArithmeticBinaryExpression(div, "idiv");
			}

			void operator()(const Node::BinExprLessThan* less) const
			{
				gen->DebugMessage("Less");
				gen->GenerateComparisonBinaryExpression(less, "jnl");
			}

			void operator()(const Node::BinExprLessThanOrEqual* lessEqual) const
			{
				gen->DebugMessage("LessThanOrEqual");
				gen->GenerateComparisonBinaryExpression(lessEqual, "jg");
			}

			void operator()(const Node::BinExprGreaterThan* greater) const
			{
				gen->DebugMessage("Greater");
				gen->GenerateComparisonBinaryExpression(greater, "jng");
			}

			void operator()(const Node::BinExprGreaterThanOrEqual* greaterEqual) const
			{
				gen->DebugMessage("GreaterThanOrEqual");
				gen->GenerateComparisonBinaryExpression(greaterEqual, "jl");
			}

			void operator()(const Node::BinExprEqual* equal) const
			{
				gen->DebugMessage("Equal");
				gen->GenerateComparisonBinaryExpression(equal, "jne");
			}

			void operator()(const Node::BinExprNotEqual* notEqual) const
			{
				gen->DebugMessage("NotEqual");
				gen->GenerateComparisonBinaryExpression(notEqual, "je");
			}
		};

		BinExprVisitor visitor{ this, expr };
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
				gen->DebugMessage("Exit");
				gen->GenerateExpression(statementExit->expr);
				gen->Move("rax", "60");
				gen->Pop("rdi");
				gen->m_Output << "\tsyscall\n";
			}
			void operator()(const Node::StatementVar* statementVar) const
			{
				gen->DebugMessage("Var");
				auto it = std::find_if(gen->m_VariableList.cbegin(), gen->m_VariableList.cend(), [&](const Variable& var) {
					return var.name == statementVar->ident.value.value();
					});
				if (it != gen->m_VariableList.cend())
				{
					THROW_ERROR_ARGS("Identifier already declared", statementVar->ident.value.value());
				}
				gen->m_VariableList.push_back({ .name = statementVar->ident.value.value(), .stackLocation = gen->m_StackSize });
				gen->GenerateExpression(statementVar->expr);
			}
			void operator()(const Node::StatementIf* statementIf) const
			{
				gen->DebugMessage("If");
				gen->m_IfCount++;
				gen->m_StatementIf = true;
				gen->GenerateExpression(statementIf->expr);
				gen->GenerateScope(statementIf->scope);
				if (statementIf->hasElse)
				{
					gen->m_EndIf = gen->m_IfCount + 1;
					gen->m_Output << "\tjmp L" << gen->m_EndIf << "\n";
				}
				gen->m_Output << "L" << gen->m_IfCount << ":\n";
				gen->m_IfCount++;
				gen->m_StatementIf = false;
			}
			void operator()(const Node::StatementElseIf* StatementElseIf) const
			{
				gen->DebugMessage("If");
				gen->m_IfCount++;
				gen->m_StatementIf = true;
				gen->GenerateExpression(StatementElseIf->expr);
				gen->GenerateScope(StatementElseIf->scope);
				if (StatementElseIf->hasElse)
					gen->m_Output << "\tjmp L" << gen->m_EndIf << "\n";
				gen->m_Output << "L" << gen->m_IfCount << ":\n";
				gen->m_StatementIf = false;
			}
			void operator()(const Node::StatementElse* statementElse) const
			{
				gen->DebugMessage("Else");
				gen->GenerateScope(statementElse->scope);
				gen->m_Output << "\nL" << gen->m_EndIf << ":\n";
			}
			void operator()(const Node::StatementScope* statementScope) const
			{
				gen->GenerateScope(statementScope);
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
		if (m_EnableDebug)
			m_Output << ";Default Exit\n";
		Move("rax", "60");
		Move("rdi", "0");
		m_Output << "\tsyscall\n";
		return m_Output.str();
	}

	void Generator::GenerateScope(const Node::StatementScope* scope)
	{
		BeginScope();
		for (const Node::Statement* stmt : scope->statement)
			GenerateStatement(stmt);
		EndScope();
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

	void Generator::BeginScope()
	{
		m_Scopes.push_back(m_VariableList.size());
	}

	void Generator::EndScope()
	{
		size_t popCount = m_VariableList.size() - m_Scopes.back();
		m_Output << "\tadd rsp," << popCount << "\n"; // Add to pop because it is reverse
		for (int i = 0; i < popCount; i++)
		{
			m_VariableList.pop_back();
		}
		m_StackSize -= popCount;
		m_Scopes.pop_back();
	}

	void Generator::DebugMessage(const std::string& message)
	{
		if (m_EnableDebug)
			m_Output << ";" << message << "\n";
	}
}
