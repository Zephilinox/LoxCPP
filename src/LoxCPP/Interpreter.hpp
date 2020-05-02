#pragma once

//STD
#include <stdexcept>
#include <vector>

//SELF
#include "Statements.hpp"
#include "Expressions.hpp"
#include "Token.hpp"
#include "Environment.hpp"

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
	void interpret(const std::vector<Statement>& statements);
	
private:
	void execute(const Statement& statement);
	Token::Literal evaluate(const Expression& expression);
	void handleStatement(const Statement& statement);
	
	bool isTruthy(const Token::Literal& literal);
	bool isEqual(const Token::Literal& left, const Token::Literal& right);

	Environment environment;
};

}