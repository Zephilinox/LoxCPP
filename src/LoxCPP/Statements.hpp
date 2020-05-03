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

struct StatementBlock;

using Statement = std::variant<
	None,
	StatementPrint,
	StatementExpression,
	StatementVariable,
	std::unique_ptr<StatementBlock>
>;

struct StatementBlock
{
	std::vector<Statement> statements;
};

}