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

using Statement = std::variant<StatementPrint, StatementExpression>;
}