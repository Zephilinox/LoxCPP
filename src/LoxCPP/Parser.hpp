#pragma once

//STD
#include <vector>
#include <cassert>
#include <stdexcept>

//Self
#include "Statements.hpp"
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
	
	std::vector<Statement> parse();

private:
	Expression expression();
	Expression equality();
	Expression comparison();
	Expression addition();
	Expression multiplication();
	Expression unary();
	Expression primary();
	Statement statement();
	Statement printStatement();
	Statement expressionStatement();
	
	template <typename... Types>
	bool match(const Types&... types)
	{
		static_assert(std::conjunction_v<std::is_same<std::decay_t<Types>, Token::Type>...>,
			"match only accepts values of Token::Type type");
		
		for (const auto* type : {&types...})
		{
			if (check(*type))
			{
				advance();
				return true;
			}
		}

		return false;
	}

	bool check(const Token::Type& type)
	{
		if (isAtEnd())
			return false;
		
		return peek().type == type;
	}

	const Token& advance()
	{
		if (!isAtEnd())
			current_token_index++;
		
		return previous();
	}

	bool isAtEnd()
	{
		return peek().type == Token::Type::EndOfFile;
	}

	const Token& peek()
	{
		assert(current_token_index >= 0);
		assert(current_token_index < static_cast<int>(tokens.size()));
		return tokens[current_token_index];
	}

	const Token& previous()
	{
		assert(current_token_index > 0);
		assert(current_token_index <= static_cast<int>(tokens.size()));
		return tokens[static_cast<std::size_t>(current_token_index) - 1];
	}

	const Token& consume(const Token::Type& type, const std::string& message)
	{
		if (check(type))
			return advance();

		//oof
		throw error(peek(), message);
	}

	ParseError error(Token token, std::string message);
	void synchronize();
	
	const std::vector<Token> tokens;
	int current_token_index = 0;
};
}
