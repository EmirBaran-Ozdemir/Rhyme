#pragma once
#include "Tokenizer.h"
#include "Core/MemoryPool.h"

namespace Node {
	struct Expr;
	struct Statement;
	struct Term;
	struct TermIntLit {
		Token intLit;
	};

	struct TermIdent {
		Token ident;
	};

	struct TermParenthesis {
		Expr* expr;
	};

	struct TermUnary {
		//std::variant<Expr*, Term*> expr;
		Expr* expr;
	};

	struct Term {
		std::variant<TermIdent*, TermIntLit*, TermParenthesis*, TermUnary*> var;
	};

	struct BinExprAddition {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprMultiplication {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprSubtraction {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprDivision {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprLessThan {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprGreaterThan {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprEqual {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprNotEqual {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprLessThanOrEqual {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExprGreaterThanOrEqual {
		Expr* lhs;
		Expr* rhs;
	};

	struct BinExpr {
		std::variant<BinExprAddition*, BinExprMultiplication*, BinExprSubtraction*, BinExprDivision*, BinExprLessThan*, BinExprGreaterThan*, BinExprEqual*, BinExprNotEqual*, BinExprLessThanOrEqual*, BinExprGreaterThanOrEqual*> binExprType;
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

	struct StatementScope {
		std::vector<Statement*> statement;
	};

	struct StatementIf {
		Expr* expr;
		StatementScope* scope;
		bool hasElse;
	};

	struct StatementElseIf {
		Expr* expr;
		StatementScope* scope;
		bool hasElse;
	};

	struct StatementElse {
		StatementScope* scope;
	};

	struct Statement {
		std::variant<StatementExit*, StatementVar*, StatementIf*, StatementElseIf*, StatementElse*, StatementScope*> var;
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
		std::optional<Node::Expr*> ParseExpr(int minPrecedence = 0, bool unary = false);
		std::optional<Node::Term*> ParseTerm(bool unary);
		std::optional<Node::StatementScope*> ParseScope();
		std::optional<Node::Statement*> ParseStatement();
		std::optional<Node::Program> ParseProgram();

		//. Statement Parsers
		void ParseExitStatement(Node::Statement* statement);
		void ParseVarStatement(Node::Statement* statement);
		void ParseIfStatement(Node::Statement* statement);
		void ParseElseStatement(Node::Statement* statement);
		void ParseElseIfStatement(Node::Statement* statement);
		void ParseElseStatementBody(Node::Statement* statement);
		void ParseScopeStatement(Node::Statement* statement);
	private:
		std::optional<Token> Peek(int offset = 0) const;
		Token Consume();
		const bool Check(TokenType type, int offset) const;
		const bool FalseCheck(TokenType type) const;
		operator std::string() { return m_Tokens[m_Index].value.value_or(""); }
	private:
		int m_Index = 0;
		std::optional<Node::Statement*> m_PrevStatement;
		Core::MemoryPool m_Pool;
		const std::vector<Token> m_Tokens;
	};
}