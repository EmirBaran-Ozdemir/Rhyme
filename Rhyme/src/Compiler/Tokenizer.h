#include "rhypch.h"
#pragma once

enum class TokenType {
	exit,
	int_lit,
	semi,
	open_paren,
	close_paren,
	ident,
	var,
	equals
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
	private:
		std::optional<char> Peek(int ahead = 0) const;
		char Consume();
	private:
		int m_Index = 0;
		std::string m_Source;
		int m_CurrentLine = 1;
		int m_CurrentPosition = 0;
	};

}