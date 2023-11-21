#include "rhypch.h"
#include "Parser.h"

namespace Compiler {

	Parser::Parser(const std::vector<Token>& tokens)
		: m_Tokens(tokens), m_Pool(1024 * 1024 * 4)
	{
		m_PrevStatement = m_Pool.Allocate<Node::Statement>();
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

	std::optional<Node::Expr*> Parser::ParseExpr(int minPrecedence, bool unary)
	{
		std::optional<Node::Term*> termLhs = ParseTerm(unary);
		if (!termLhs.has_value())
			return std::nullopt;

		auto exprLhs = m_Pool.Allocate<Node::Expr>();
		exprLhs->var = termLhs.value();
		while (true)
		{
			std::optional<Token> currentToken = Peek();
			std::optional<int> precedence;
			if (currentToken.has_value())
			{
				precedence = Tokenizer::GetBinaryPrecedence(currentToken->type);
				if (!precedence.has_value() || precedence < minPrecedence)
					break;
			}
			else
				break;

			Token op = Consume();
			int nextMinPrec = precedence.value() + 1;
			auto exprRhs = ParseExpr(nextMinPrec);
			if (!exprRhs.has_value())
				THROW_ERROR("No right hand side");

			auto binExpr = m_Pool.Allocate<Node::BinExpr>();
			auto exprLhsTemp = m_Pool.Allocate<Node::Expr>();

			auto createBinaryExpr = [&](auto binExprType) {
				exprLhsTemp->var = exprLhs->var;
				binExprType->lhs = exprLhsTemp;
				binExprType->rhs = exprRhs.value();
				binExpr->binExprType = binExprType;
			};
			if (unary)
			{
				auto invertedToken = Tokenizer::InvertToken(op.type);
				if (invertedToken.has_value())
					op.type = invertedToken.value();
				else
					THROW_ERROR("Invalid unary operator");
			}
			switch (op.type) {
				case TokenType::Plus:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprAddition>());
					break;
				case TokenType::Star:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprMultiplication>());
					break;
				case TokenType::Minus:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprSubtraction>());
					break;
				case TokenType::Slash:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprDivision>());
					break;
				case TokenType::Less:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprLessThan>());
					break;
				case TokenType::LessEqual:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprLessThanOrEqual>());
					break;
				case TokenType::Greater:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprGreaterThan>());
					break;
				case TokenType::GreaterEqual:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprGreaterThanOrEqual>());
					break;
				case TokenType::Equal:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprEqual>());
					break;
				case TokenType::NotEqual:
					createBinaryExpr(m_Pool.Allocate<Node::BinExprNotEqual>());
					break;
			}
			exprLhs->var = binExpr;
		}

		return exprLhs;
	}

	std::optional<Node::Term*> Parser::ParseTerm(bool unary)
	{
		auto createTerm = [&](auto termType)
		{
			auto term = m_Pool.Allocate<Node::Term>();
			term->var = termType;
			return term;
		};
		if (Check(TokenType::IntegerLiteral))
		{
			auto termIntLit = m_Pool.Allocate<Node::TermIntLit>();
			termIntLit->intLit = Consume();
			return createTerm(termIntLit);
		}
		else if (Check(TokenType::Ident))
		{
			auto termIdent = m_Pool.Allocate<Node::TermIdent>();
			termIdent->ident = Consume();
			return createTerm(termIdent);
		}
		else if (Check(TokenType::OpenParenthesis))
		{
			Consume(); // Consume '('
			auto expr = ParseExpr(0, unary);
			if (!expr.has_value())
				THROW_ERROR("Expected expression");
			if (Check(TokenType::CloseParenthesis))
				Consume(); // Consume ')'
			else
				THROW_ERROR("Missing ')'");
			auto termParenthesis = m_Pool.Allocate<Node::TermParenthesis>();
			termParenthesis->expr = expr.value();
			return createTerm(termParenthesis);
		}
		else if (Check(TokenType::Exclamation))
		{
			Consume(); // Consume '!'
			if (Check(TokenType::OpenParenthesis))
			{
				unary = unary ? false : true;
				auto expr = ParseExpr(0, unary);
				if (!expr.has_value())
					THROW_ERROR("Expected expression");

				auto termUnary = m_Pool.Allocate<Node::TermUnary>();
				termUnary->expr = expr.value();
				return createTerm(termUnary);
			}
			else
				THROW_ERROR("Missing ')'");
		}
		else
			return std::nullopt;
	}

	std::optional<Node::StatementScope*> Parser::ParseScope()
	{
		if (FalseCheck(TokenType::OpenCurlyParenthesis))
			THROW_ERROR("Missing open curly parenthesis");
		Consume(); // Consume '{'
		auto scopeStatement = m_Pool.Allocate<Node::StatementScope>();
		while (auto statement = ParseStatement())
		{
			scopeStatement->statement.push_back(statement.value());
		}
		if (FalseCheck(TokenType::CloseCurlyParenthesis))
			THROW_ERROR("Missing close curly parenthesis");
		Consume(); // Consume '}'
		return scopeStatement;
	}

	std::optional<Node::Statement*> Parser::ParseStatement() {
		auto statement = m_Pool.Allocate<Node::Statement>();

		if (Check(TokenType::Exit)) {
			Consume(); // Consume 'exit'

			if (FalseCheck(TokenType::OpenParenthesis))
				THROW_ERROR("Missing open parenthesis");

			Consume(); // Consume '('

			auto exitStatement = m_Pool.Allocate<Node::StatementExit>();

			auto expr = ParseExpr();

			if (!expr.has_value())
				THROW_ERROR("Invalid expression");

			exitStatement->expr = expr.value();

			if (FalseCheck(TokenType::CloseParenthesis))
				THROW_ERROR("Missing close parenthesis");

			Consume(); // Consume ')'
			if (FalseCheck(TokenType::Semicolon))
				THROW_ERROR("Missing semicolon");
			Consume(); // Consume ';'

			statement->var = exitStatement;
		}
		else if (Check(TokenType::Variable) && Check(TokenType::Ident, 1) && Check(TokenType::Assign, 2)) {
			Consume(); // Consume 'var'

			auto varStatement = m_Pool.Allocate<Node::StatementVar>();

			varStatement->ident = Consume(); // Consume identifier
			Consume(); // Consume '='

			auto expr = ParseExpr(); // Parse and consume the expression

			if (!expr.has_value())
				THROW_ERROR("Invalid expression");

			if (FalseCheck(TokenType::Semicolon))
				THROW_ERROR("Missing semicolon");
			Consume(); // Consume ';'
			varStatement->expr = expr.value();
			statement->var = varStatement;
		}
		else if (Check(TokenType::If))
		{
			auto ifStatement = m_Pool.Allocate<Node::StatementIf>();

			Consume(); // Consume 'if'

			if (FalseCheck(TokenType::OpenParenthesis))
				THROW_ERROR("Missing open parenthesis");
			Consume(); // Consume '('

			auto expr = ParseExpr();
			if (!expr.has_value())
				THROW_ERROR("Invalid expression");

			ifStatement->expr = expr.value();

			if (FalseCheck(TokenType::CloseParenthesis))
				THROW_ERROR("Missing close parenthesis");
			Consume(); // Consume ')'

			if (auto scope = ParseScope())
				ifStatement->scope = scope.value();
			else
				THROW_ERROR("Expected scope");

			if (Check(TokenType::Else))
			{
				ifStatement->hasElse = true;
			}
			statement->var = ifStatement;
		}

		else if (Check(TokenType::Else))
		{
			auto index = m_PrevStatement.value()->var.index();
			if (!std::holds_alternative<Node::StatementIf*>(m_PrevStatement.value()->var) && !std::holds_alternative<Node::StatementElseIf*>(m_PrevStatement.value()->var))
				THROW_ERROR("If statement required before else statement");

			Consume(); // Consume 'else'
			if (Check(TokenType::If))
			{
				auto ifStatement = m_Pool.Allocate<Node::StatementElseIf>();

				Consume(); // Consume 'if'

				if (FalseCheck(TokenType::OpenParenthesis))
					THROW_ERROR("Missing open parenthesis");
				Consume(); // Consume '('

				auto expr = ParseExpr();
				if (!expr.has_value())
					THROW_ERROR("Invalid expression");

				ifStatement->expr = expr.value();

				if (FalseCheck(TokenType::CloseParenthesis))
					THROW_ERROR("Missing close parenthesis");
				Consume(); // Consume ')'

				if (auto scope = ParseScope())
					ifStatement->scope = scope.value();
				else
					THROW_ERROR("Expected scope");

				if (Check(TokenType::Else))
				{
					ifStatement->hasElse = true;
				}
				statement->var = ifStatement;
			}
			else
			{
				auto elseStatement = m_Pool.Allocate<Node::StatementElse>();
				if (auto scope = ParseScope())
					elseStatement->scope = scope.value();
				else
					THROW_ERROR("Expected scope");
				statement->var = elseStatement;
			}
		}
		else if (Check(TokenType::OpenCurlyParenthesis)) //! Scope
		{
			if (auto scopeStatement = ParseScope())
			{
				statement->var = scopeStatement.value();
				return statement;
			}
			else
				THROW_ERROR("Invalid scope");
		}
		else
			return std::nullopt;

		Node::Statement* retStatement = new Node::Statement{ .var = statement->var };
		m_PrevStatement = retStatement;
		return retStatement;
	}

	std::optional<Node::Program> Parser::ParseProgram() {
		Node::Program program;
		while (Peek().has_value())
		{
			auto statement = ParseStatement();
			if (statement.has_value())
				program.statement.push_back(statement.value());
			else
				THROW_ERROR("Invalid statement");
		}
		return program;
	}

	//void Parser::Handler::Exception::ThrowError(const std::string& message)
	//{
	//	throw std::invalid_argument(fmt::format("ERROR::PARSER::{} at line: {} at position: {}", message, m_Tokens[m_Index].line, m_Tokens[m_Index].position));
	//}
	//void Parser::Handler::Exception::ThrowError(const std::string& message, const std::string& expression)
	//{
	//	throw std::invalid_argument(fmt::format("ERROR::PARSER::{} {} at line: {} at position: {}", message, expression, m_Tokens[m_Index].line, m_Tokens[m_Index].position));
	//}
}