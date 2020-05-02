#pragma once

//STD
#include <unordered_map>

//SELF
#include "Token.hpp"

namespace LoxCPP
{
class Environment
{
public:
	void define(std::string name, Token::Literal value);
	void assign(Token name, Token::Literal value);

	Token::Literal get(const Token& name) const;

private:
	std::unordered_map<std::string, Token::Literal> values;
};
}