#pragma once
#include "Tokenizer.h"
#include "MemoryPool.h"

namespace Node {
	struct TermIntLit {
		Token intLit;
	};

	struct TermIdent {
		Token ident;
	};

	struct Expr;
	struct BinExprAddition {
		Expr* lhs;
		Expr* rhs;
	};

	//struct BinExprMultiplication {
	//	Expr* lhs;
	//	Expr* rhs;
	//};

	struct BinExpr {
		BinExprAddition* add;
	};

	struct Term {
		std::variant<TermIdent*, TermIntLit*> var;
	};

	struct Expr {
		std::variant<Term*, BinExpr*> var;
	};
	struct StatementExit {
		Expr* expr;
	};

	struct StatementVar {
		Token ident;
		Expr* expr;
	};

	struct Statement {
		std::variant<StatementExit*, StatementVar*> var;
	};

	struct Program {
		std::vector<Statement*> statement;
	};
}
namespace Compiler
{
	class Parser
	{
	public:
		Parser(const std::vector<Token>& tokens);
		std::optional<Node::Expr*> ParseExpr();
		std::optional<Node::Term*> ParseTerm();
		std::optional<Node::Statement*> ParseStatement();
		std::optional<Node::Program> ParseProgram();
	private:
		std::optional<Token> Peek(int ahead = 0) const;
		Token Consume();
		const bool Check(TokenType type, int offset) const;
		const bool FalseCheck(TokenType type) const;

		operator std::string() { return m_Tokens[m_Index].value.value_or(""); }
		void ThrowError(const std::string& message);
		void ThrowError(const std::string& message, const std::string& expression);
	private:
		int m_Index = 0;
		Resources::MemoryPool m_Pool;
		const std::vector<Token> m_Tokens;
	};
}