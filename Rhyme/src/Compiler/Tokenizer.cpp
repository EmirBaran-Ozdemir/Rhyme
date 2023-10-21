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
					tokens.push_back({ .type = TokenType::exit, .line = m_CurrentLine, .position = m_CurrentPosition });
					buffer.clear();
				}
				else if (buffer == "var")
				{
					tokens.push_back({ .type = TokenType::var, .line = m_CurrentLine, .position = m_CurrentPosition });
					buffer.clear();
				}
				else
				{
					tokens.push_back({ .type = TokenType::ident, .value = buffer, .line = m_CurrentLine, .position = m_CurrentPosition });
					buffer.clear();
				}
			}
			else if (Peek().value() == '(')
			{
				Consume();
				tokens.push_back({ .type = TokenType::open_paren, .line = m_CurrentLine, .position = m_CurrentPosition });
			}
			else if (Peek().value() == ')')
			{
				Consume();
				tokens.push_back({ .type = TokenType::close_paren, .line = m_CurrentLine, .position = m_CurrentPosition });
			}
			else if (std::isdigit(Peek().value()))
			{
				buffer.push_back(Consume());
				while (Peek().has_value() && std::isdigit(Peek().value()))
				{
					buffer.push_back(Consume());
				}
				tokens.push_back({ .type = TokenType::int_lit, .value = buffer, .line = m_CurrentLine, .position = m_CurrentPosition });
				buffer.clear();
			}
			else if (Peek().value() == '=')
			{
				Consume();
				tokens.push_back({ .type = TokenType::equals, .line = m_CurrentLine, .position = m_CurrentPosition });
			}
			else if (Peek().value() == ';')
			{
				tokens.push_back({ .type = TokenType::semi, .line = m_CurrentLine, .position = m_CurrentPosition });
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
