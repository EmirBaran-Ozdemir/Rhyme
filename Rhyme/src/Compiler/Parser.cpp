#include "rhypch.h"
#include "Parser.h"

namespace Compiler {

	Parser::Parser(const std::vector<Token>& tokens)
		: m_Tokens(tokens), m_Pool(1024 * 1024 * 4)
	{

	}

	std::optional<Token> Parser::Peek(int offset) const
	{
		if (m_Index + offset >= m_Tokens.size())
			return std::nullopt;
		else
			return m_Tokens[m_Index + offset];
	}

	Token Parser::Consume()
	{
		return m_Tokens[m_Index++];
	}

	const bool Parser::Check(TokenType type, int offset = 0) const
	{
		return Peek(offset).has_value() && Peek(offset).value().type == type;
	}

	const bool Parser::FalseCheck(TokenType type) const
	{
		return !Peek().has_value() || Peek().value().type != type;
	}

	std::optional<Node::Expr*> Parser::ParseExpr()
	{
		if (auto term = ParseTerm())
		{
			if (FalseCheck(TokenType::Plus))
			{
				auto expr = m_Pool.Allocate<Node::Expr>();
				expr->var = term.value();
				return expr;
			}
			auto plus = Consume(); // +
			auto binExpr = m_Pool.Allocate<Node::BinExpr>();
			auto binExprAdd = m_Pool.Allocate<Node::BinExprAddition>();
			auto lhs = m_Pool.Allocate<Node::Expr>();
			lhs->var = term.value();
			binExprAdd->lhs = lhs;
			if (auto rhs = ParseExpr())
			{
				binExprAdd->rhs = rhs.value();
				binExpr->add = binExprAdd;
				auto expr = m_Pool.Allocate<Node::Expr>();
				expr->var = binExpr;
				return expr;
			}
			else
				ThrowError("Expected expression");
		}
		else
			return std::nullopt;
	}

	std::optional<Node::Term*> Parser::ParseTerm()
	{
		if (Check(TokenType::IntegerLiteral))
		{
			auto termIntLit =m_Pool.Allocate<Node::TermIntLit>();
			termIntLit->intLit = Consume();
			auto term= m_Pool.Allocate<Node::Term>();
			term->var = termIntLit;
			return term;
		}
		else if (Check(TokenType::Ident))
		{
			auto termIdent = m_Pool.Allocate<Node::TermIdent>();
			termIdent->ident = Consume();
			auto term = m_Pool.Allocate<Node::Term>();
			term->var = termIdent;
			return term;
		}
		else
			return std::nullopt;
	}

	std::optional<Node::Statement*> Parser::ParseStatement() {
		Node::Statement* statement = static_cast<Node::Statement*>(m_Pool.Allocate<Node::Statement>());

		if (Check(TokenType::Exit)) {
			Consume(); // Consume 'exit'

			if (FalseCheck(TokenType::OpenParenthesis))
				ThrowError("Missing open parenthesis");


			Consume(); // Consume '('

			Node::StatementExit* exitStatement = static_cast<Node::StatementExit*>(m_Pool.Allocate<Node::StatementExit>());
			auto expr = ParseExpr();

			if (!expr.has_value())
				ThrowError("Invalid expression", *this);


			exitStatement->expr = expr.value();

			if (FalseCheck(TokenType::CloseParenthesis))
				ThrowError("Missing close parenthesis");


			Consume(); // Consume ')'

			statement->var = exitStatement;
		}
		else if (Check(TokenType::Variable) && Check(TokenType::Ident, 1) && Check(TokenType::Equals, 2)) {
			Consume(); // Consume 'var'

			Node::StatementVar* varStatement = static_cast<Node::StatementVar*>(m_Pool.Allocate<Node::StatementVar>());
			varStatement->ident = Consume(); // Consume identifier
			Consume(); // Consume '='

			auto expr = ParseExpr(); // Parse and consume the expression

			if (!expr.has_value())
				ThrowError("Invalid expression", *this);


			varStatement->expr = expr.value();
			statement->var = varStatement;
		}
		else if (Check(TokenType::Variable) && Check(TokenType::Plus, 1) && Check(TokenType::Variable, 2))
		{
			Token value = Consume();
		}
		else
			return std::nullopt;


		if (FalseCheck(TokenType::Semicolon))
			ThrowError("Missing semicolon");

		Consume(); // Consume ';'

		Node::Statement* retStatement = new Node::Statement{ .var = statement->var };
		return retStatement;
	}

	std::optional<Node::Program> Parser::ParseProgram() {
		Node::Program program;
		while (Peek().has_value())
		{
			if (auto statement = ParseStatement())
				program.statement.push_back(statement.value());
			else
				ThrowError("Invalid statement");
		}
		return program;
	}

	void Parser::ThrowError(const std::string& message)
	{
		throw std::invalid_argument("ERROR::PARSER::" + message + " at line: " + std::to_string(m_Tokens[m_Index].line) + " at position: " + std::to_string(m_Tokens[m_Index].position));
	}
	void Parser::ThrowError(const std::string& message, const std::string& expression)
	{
		throw std::invalid_argument("ERROR::PARSER::" + message + " " + expression + " at line: " + std::to_string(m_Tokens[m_Index].line) + " at position: " + std::to_string(m_Tokens[m_Index].position));
	}
}