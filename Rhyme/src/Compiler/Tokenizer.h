#include "rhypch.h"
#pragma once

enum class TokenType {
	Exit,
	IntegerLiteral,
	Semicolon,
	OpenParenthesis,
	CloseParenthesis,
	Ident,
	Variable,
	Equals,
	Plus,
	Star,
	Minus,
	Slash,
	Backslash,
	LessThan,
	GreaterThan
};

struct Token {
	TokenType type;
	std::optional<std::string> value {};
	int line;
	int position;
};

namespace Compiler
{

	class Tokenizer
	{
	public:
		Tokenizer(const std::string& src);
		std::vector<Token> Tokenize(std::string source);
		static bool IsBinOperator(TokenType type);
		static std::optional<int> GetBinaryPrecedence(TokenType type);
	private:
		std::optional<char> Peek(int ahead = 0) const;
		char Consume();
		void PushBack(std::vector<Token>& tokens, TokenType token);
		void PushBack(std::vector<Token>& tokens, TokenType token, std::string& buffer);
	private:
		int m_Index = 0;
		std::string m_Source;
		int m_CurrentLine = 1;
		int m_CurrentPosition = 0;
	};

}