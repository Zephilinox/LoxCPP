#pragma once

//STD
#include <stdexcept>

//SELF
#include "Expressions.hpp"
#include "Token.hpp"

namespace LoxCPP
{

class RuntimeError : public std::runtime_error
{
public:
	//todo: reference
	Token token;
	
	RuntimeError(Token token, const std::string& err)
		: std::runtime_error(err)
		, token(std::move(token))
	{}
};

class Interpreter
{
public:
	void interpret(const Expression& expression);
	
private:
	Token::Literal evaluate(const Expression& expression);
	bool isTruthy(const Token::Literal& literal);
	bool isEqual(const Token::Literal& left, const Token::Literal& right);
};

}