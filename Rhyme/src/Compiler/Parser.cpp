#include "rhypch.h"
#include "Parser.h"

namespace Compiler {

	Parser::Parser(const std::vector<Token>& tokens)
		: m_Tokens(tokens)
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

	std::optional<Node::Expr> Parser::ParseExpr()
	{
		if (Check(TokenType::int_lit))
			return Node::Expr{.var = Node::ExprIntLit{ .int_lit = Consume() } };
		else if (Check(TokenType::ident))
			return Node::Expr{.var = Node::ExprIdent{ .ident = Consume() } };
		else
			return std::nullopt;
	}

	std::optional<Node::Statement> Parser::ParseStatement()
	{
		Node::Statement _var;
		if (Check(TokenType::exit))
		{
			Consume(); //? Consume exit

			if (FalseCheck(TokenType::open_paren))
				ThrowError("Missing open paranthesis");

			Consume(); //? Consume '('

			Node::StatementExit _stExit;
			auto expr = ParseExpr();

			if (expr)
				_stExit = { .expr = expr.value() };
			else
				ThrowErrorEx("Invalid expression", *this);
			
			if (FalseCheck(TokenType::close_paren))
				ThrowError("Missing close paranthesis");

			Consume(); //? Consume ')'

			if (FalseCheck(TokenType::semi))
				ThrowError("Missing semicolon");

			Consume(); //? Consume ';'
			_var.var = _stExit;
		}
		else if (Check(TokenType::var) && Check(TokenType::ident, 1) && Check(TokenType::equals, 2))
		{
			Consume(); //? Consume var
			Node::StatementVar _stVar;
			_stVar.ident = Consume(); //? Consume ident
			Consume(); //? Consume equals
			auto expr = ParseExpr(); //? Parse && consume expression

			if (expr)
				_stVar.expr = expr.value();
			else
				ThrowErrorEx("Invalid expression", *this);

			if (Check(TokenType::semi))
				Consume(); //? Consume ';'
			else
				ThrowError("Missing semicolon");
			_var.var = _stVar;
		}
		else
			return std::nullopt;

		return Node::Statement{.var = _var.var};
	}

	std::optional<Node::Program> Parser::ParseProgram(){
		Node::Program program;
		while (Peek().has_value())
		{
			if(auto statement = ParseStatement())
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
	void Parser::ThrowErrorEx(const std::string& message, const std::string& expression)
	{
		throw std::invalid_argument("ERROR::PARSER::" + message + " " + expression + " at line: " + std::to_string(m_Tokens[m_Index].line) + " at position: " + std::to_string(m_Tokens[m_Index].position));
	}
}