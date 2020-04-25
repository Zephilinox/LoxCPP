#pragma once

//STD
#include <vector>
#include <cassert>
#include <stdexcept>

//Self
#include "Expressions.hpp"
#include "Token.hpp"

namespace LoxCPP
{

class ParseError : public std::runtime_error
{
public:
	ParseError() : std::runtime_error("") {}
	ParseError(std::string err) : std::runtime_error(err) {}
};

class Parser
{
public:
	Parser(std::vector<Token>&& tokens);
	
	Expression parse();

private:
	Expression expression();
	Expression equality();
	Expression comparison();
	Expression addition();
	Expression multiplication();
	Expression unary();
	Expression primary();
	
	template <typename... Types>
	bool match(Types... types)
	{
		static_assert(std::conjunction_v<std::is_same<Types, Token::Type>...>,
			"match only accepts values of Token::Type type");
		
		std::array types_in_array{types...};
		for (const auto& type : types_in_array)
		{
			if (check(type))
			{
				advance();
				return true;
			}
		}

		return false;
	}

	bool check(Token::Type type)
	{
		if (isAtEnd())
			return false;
		
		return peek().type == type;
	}

	Token advance()
	{
		if (!isAtEnd())
			current_token_index++;
		
		return previous();
	}

	bool isAtEnd()
	{
		return peek().type == Token::Type::EndOfFile;
	}

	Token peek()
	{
		assert(current_token_index >= 0);
		assert(current_token_index < static_cast<int>(tokens.size()));
		return tokens[current_token_index];
	}

	Token previous()
	{
		assert(current_token_index - 1 >= 0);
		assert(current_token_index - 1 < static_cast<int>(tokens.size()));
		return tokens[current_token_index - 1];
	}

	Token consume(Token::Type type, std::string message)
	{
		if (check(type))
			return advance();

		//oof
		throw error(peek(), std::move(message));
	}

	ParseError error(Token token, std::string message);
	void synchronize();
	
	std::vector<Token> tokens;
	int current_token_index = 0;
};
}
