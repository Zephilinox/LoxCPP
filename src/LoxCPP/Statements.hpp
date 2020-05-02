#pragma once

//STD
#include <variant>

//SELF
#include "Expressions.hpp"

namespace LoxCPP
{
struct StatementPrint
{
	Expression expression;
};

struct StatementExpression
{
	Expression expression;
};

struct StatementVariable
{
	Token name;
	Expression initializer;
};

using Statement = std::variant<
	None,
	StatementPrint,
	StatementExpression,
	StatementVariable
>;

}