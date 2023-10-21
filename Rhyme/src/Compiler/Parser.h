#pragma once
#include "Tokenizer.h"

namespace Node {
	struct ExprIntLit {
		Token int_lit;
	};

	struct ExprIdent {
		Token ident;
	};

	struct Expr {
		std::variant<ExprIdent, ExprIntLit> var;
	};

	struct StatementExit {
		Expr expr;
	};

	struct StatementVar {
		Token ident;
		Expr expr;
	};

	struct Statement {
		std::variant<StatementExit, StatementVar> var;
	};

	struct Program {
		std::vector<Statement> statement;
	};
}
namespace Compiler
{
	class Parser
	{
	public:
		Parser(const std::vector<Token>& tokens);
		std::optional<Node::Expr> ParseExpr();
		std::optional<Node::Statement> ParseStatement();
		std::optional<Node::Program> ParseProgram();
	private:
		std::optional<Token> Peek(int ahead = 0) const;
		Token Consume();
		const bool Check(TokenType type, int offset) const;
		const bool FalseCheck(TokenType type) const;

		operator std::string() { return m_Tokens[m_Index].value.value_or(""); }
		void ThrowError(const std::string& message);
		void ThrowErrorEx(const std::string& message,const std::string& expression);
	private:
		int m_Index = 0;
		const std::vector<Token> m_Tokens;

	};
}