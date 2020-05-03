#pragma once

//STD
#include <unordered_map>
#include <memory>

//SELF
#include "Token.hpp"

namespace LoxCPP
{
class Environment
{
public:
	void define(std::string name, Token::Literal value);
	void assign(const Token& name, Token::Literal value);

	[[nodiscard]] Token::Literal get(const Token& name) const;

	Environment* parent = nullptr;

private:
	std::unordered_map<std::string, Token::Literal> values;
};
}