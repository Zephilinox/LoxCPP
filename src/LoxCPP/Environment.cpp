#include "Environment.hpp"

//SELF
#include "Interpreter.hpp"

namespace LoxCPP
{
void Environment::define(std::string name, Token::Literal value)
{
	values[std::move(name)] = std::move(value);
}
Token::Literal Environment::get(const Token& name) const
{
	const auto it = values.find(name.lexeme);

	if (it == values.end())
		throw RuntimeError(name, "Undefined variable'" + name.lexeme + "'.");

	return it->second;
}
}
