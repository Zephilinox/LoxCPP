#pragma once

//STD
#include <vector>
#include <unordered_map>

//SELF
#include "Token.hpp"

namespace LoxCPP
{

class Lexer
{
public:
	Lexer(std::string source);
	
	std::vector<Token> generateTokens();
	
private:
	bool isAtEnd() const
	{
		return current >= source.length();
	}

	bool isDigit(char c) const
	{
		return c >= '0' && c <= '9';
	}

	bool isAlpha(char c) const
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}

	bool isAlphaNumeric(char c) const
	{
		return isDigit(c) || isAlpha(c);
	}

	char advance();
	void scanToken();
	void addToken(Token::Type type);
	void addToken(Token::Type type, Token::Literal literal);
	char peek(int offset = 0);
	bool match(char expected);
	void handleStringCharacter();
	void handleNumberCharacter();
	void handleIdentifierCharacter();
	
	std::string source;
	std::vector<Token> tokens;

	int start = 0;
	int current = 0;
	int line = 1;
};

}