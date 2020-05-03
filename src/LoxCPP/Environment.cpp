#include "Environment.hpp"

//SELF
#include "Interpreter.hpp"

namespace LoxCPP
{
void Environment::define(std::string name, Token::Literal value)
{
	values[std::move(name)] = std::move(value);
}

void Environment::assign(const Token& name, Token::Literal value)
{
	const auto it = values.find(name.lexeme);

	if (it != values.end())
	{
		it->second = std::move(value);
		return;
	}

	if (parent)
	{
		parent->assign(std::move(name), std::move(value));
		return;
	}

	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

Token::Literal Environment::get(const Token& name) const
{
	const auto it = values.find(name.lexeme);

	if (it != values.end())
		return it->second;

	if (parent)
		return parent->get(name);
	
	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");

}
}
