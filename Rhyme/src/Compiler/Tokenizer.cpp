#include "rhypch.h"
#include "Tokenizer.h"

namespace Compiler
{

	Tokenizer::Tokenizer(const std::string& src)
		: m_Source(src) {}

	std::optional<char> Tokenizer::Peek(int offset) const
	{
		if (m_Index + offset >= m_Source.length())
			return std::nullopt;
		else
			return m_Source[m_Index + offset];
	}

	char Tokenizer::Consume()
	{
		m_CurrentPosition++;
		return m_Source[m_Index++];
	}

	void Tokenizer::PushBack(std::vector<Token>& tokens, TokenType tokenType)
	{
		tokens.push_back({ .type = tokenType, .line = m_CurrentLine, .position = m_CurrentPosition - 1 });
	}

	void Tokenizer::PushBack(std::vector<Token>& tokens, TokenType tokenType, std::string& buffer)
	{
		tokens.push_back({ .type = tokenType, .value = buffer, .line = m_CurrentLine, .position = m_CurrentPosition - 1 });
	}

	bool Tokenizer::IsBinOperator(TokenType type)
	{
		switch (type)
		{
			case TokenType::Less:			return true;
			case TokenType::LessEqual:		return true;
			case TokenType::Greater:		return true;
			case TokenType::GreaterEqual:	return true;
			case TokenType::Equal:			return true;
			case TokenType::NotEqual:		return true;
			case TokenType::Exclamation:	return true;
			case TokenType::Plus:			return true;
			case TokenType::Minus:			return true;
			case TokenType::Star:			return true;
			case TokenType::Slash:			return true;
		}
		return false;
	}

	std::optional<int> Tokenizer::GetBinaryPrecedence(TokenType type)
	{
		switch (type)
		{
			case TokenType::Less:			return 0;
			case TokenType::LessEqual:		return 0;
			case TokenType::Greater:		return 0;
			case TokenType::GreaterEqual:	return 0;
			case TokenType::Equal:			return 0;
			case TokenType::NotEqual:		return 0;
			case TokenType::Plus:			return 1;
			case TokenType::Minus:			return 1;
			case TokenType::Star:			return 2;
			case TokenType::Slash:			return 2;
			case TokenType::Exclamation:	return 3;
		}
		return std::nullopt;
	}

	std::optional<TokenType> Tokenizer::InvertToken(TokenType type)
	{
		switch (type)
		{
			case TokenType::Less:			return TokenType::GreaterEqual;
			case TokenType::LessEqual:		return TokenType::Greater;
			case TokenType::Greater:		return TokenType::LessEqual;
			case TokenType::GreaterEqual:	return TokenType::Less;
			case TokenType::Equal:			return TokenType::NotEqual;
			case TokenType::NotEqual:		return TokenType::Equal;
		}
		return std::nullopt;
	}

	std::vector<Token> Tokenizer::Tokenize(std::string source)
	{
		std::vector<Token> tokens;
		std::string buffer;


		while (Peek().has_value())
		{
			if (std::isalpha(Peek().value()))
			{
				buffer.push_back(Consume());
				while (Peek().has_value() && std::isalnum(Peek().value()))
				{
					buffer.push_back(Consume());
				}
				if (buffer == "exit")
				{
					PushBack(tokens, TokenType::Exit);
					buffer.clear();
				}
				else if (buffer == "var")
				{
					PushBack(tokens, TokenType::Variable);
					buffer.clear();
				}
				else if (buffer == "if")
				{
					PushBack(tokens, TokenType::If);
					buffer.clear();
				}
				else if (buffer == "else")
				{
					PushBack(tokens, TokenType::Else);
					buffer.clear();
				}
				else
				{
					PushBack(tokens, TokenType::Ident, buffer);
					buffer.clear();
				}
			}
			else if (Peek().value() == '(')
			{
				Consume();
				PushBack(tokens, TokenType::OpenParenthesis);
			}
			else if (Peek().value() == ')')
			{
				Consume();
				PushBack(tokens, TokenType::CloseParenthesis);
			}
			else if (Peek().value() == '{')
			{
				Consume();
				PushBack(tokens, TokenType::OpenCurlyParenthesis);
			}
			else if (Peek().value() == '}')
			{
				Consume();
				PushBack(tokens, TokenType::CloseCurlyParenthesis);
			}
			else if (Peek().value() == '+')
			{
				Consume();
				PushBack(tokens, TokenType::Plus);
			}
			else if (Peek().value() == '*')
			{
				Consume();
				PushBack(tokens, TokenType::Star);
			}
			else if (Peek().value() == '-')
			{
				Consume();
				PushBack(tokens, TokenType::Minus);
			}
			else if (Peek().value() == '/')
			{
				Consume();
				if (Peek().value() == '/')
				{
					Consume();
					while (Peek().has_value() && Peek().value() != '\n')
						Consume();
				}
				else
					PushBack(tokens, TokenType::Slash);
			}
			else if (Peek().value() == '<')
			{
				Consume();
				if (Peek().value() == '=')
				{
					Consume();
					PushBack(tokens, TokenType::LessEqual);
				}
				else
					PushBack(tokens, TokenType::Less);
			}
			else if (Peek().value() == '>')
			{
				Consume();
				if (Peek().value() == '=')
				{
					Consume();
					PushBack(tokens, TokenType::GreaterEqual);
				}
				else
					PushBack(tokens, TokenType::Greater);
			}
			else if (Peek().value() == '!')
			{
				Consume();
				if (Peek().value() == '=')
				{
					Consume();
					PushBack(tokens, TokenType::NotEqual);
				}
				else
					PushBack(tokens, TokenType::Exclamation);
			}
			else if (Peek().value() == '=')
			{
				Consume();
				if (Peek().value() == '=')
				{
					Consume();
					PushBack(tokens, TokenType::Equal);
				}
				else
					PushBack(tokens, TokenType::Assign);
			}
			else if (Peek().value() == ';')
			{
				PushBack(tokens, TokenType::Semicolon);
				Consume();
			}
			else if (std::isdigit(Peek().value()))
			{
				buffer.push_back(Consume());
				while (Peek().has_value() && std::isdigit(Peek().value()))
				{
					buffer.push_back(Consume());
				}
				PushBack(tokens, TokenType::IntegerLiteral, buffer);
				buffer.clear();
			}
			else if (Peek().value() == '\n')
			{
				Consume();
				m_CurrentLine++;
				m_CurrentPosition = 0;
			}
			else if (std::isspace(Peek().value()))
			{
				Consume();
			}
			else
			{
				THROW_ERROR_ARGS("Unexpected character {} at line: {}, at position: {}", m_Source[m_Index], m_CurrentLine, m_CurrentPosition);
			}
		}
		m_Index = 0;
		return tokens;
	}
}
