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
		tokens.push_back({ .type = tokenType, .line = m_CurrentLine, .position = m_CurrentPosition });
	}

	void Tokenizer::PushBack(std::vector<Token>& tokens, TokenType tokenType, std::string& buffer)
	{
		tokens.push_back({ .type = tokenType, .value = buffer, .line = m_CurrentLine, .position = m_CurrentPosition });
	}

	bool Tokenizer::IsBinOperator(TokenType type)
	{
		switch (type)
		{
			case TokenType::Star:			return true;
			case TokenType::Plus:			return true;
			case TokenType::Minus:			return true;
			case TokenType::Slash:			return true;
			case TokenType::GreaterThan:	return true;
			case TokenType::LessThan:		return true;

			default: return false;
		}
	}

	std::optional<int> Tokenizer::GetBinaryPrecedence(TokenType type)
	{
		switch (type)
		{
			case TokenType::GreaterThan:	return 0;
			case TokenType::LessThan:		return 0;
			case TokenType::Plus:			return 1;
			case TokenType::Minus:			return 1;
			case TokenType::Star:			return 2;
			case TokenType::Slash:			return 2;

			default: return std::nullopt;
		}
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
				PushBack(tokens, TokenType::Slash);
			}
			else if (Peek().value() == '<')
			{
				Consume();
				PushBack(tokens, TokenType::LessThan);
			}
			else if (Peek().value() == '>')
			{
				Consume();
				PushBack(tokens, TokenType::GreaterThan);
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
			else if (Peek().value() == '=')
			{
				Consume();
				PushBack(tokens, TokenType::Equals);
			}
			else if (Peek().value() == ';')
			{
				PushBack(tokens, TokenType::Semicolon);
				Consume();
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
				throw std::invalid_argument("ERROR::TOKENIZER::Unexpected character '" + std::string(1, m_Source[m_Index]) + "' at line: " + std::to_string(m_CurrentLine) + ", position: " + std::to_string(m_CurrentPosition));
			}
		}
		m_Index = 0;
		return tokens;
	}
}
