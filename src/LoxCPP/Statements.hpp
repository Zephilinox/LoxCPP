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
struct StatementIf;
struct StatementWhile;

using Statement = std::variant<
	None,
	StatementPrint,
	StatementExpression,
	StatementVariable,
	std::unique_ptr<StatementBlock>,
	std::unique_ptr<StatementWhile>,
	std::unique_ptr<StatementIf>
>;

struct StatementBlock
{
	std::vector<Statement> statements;
};

struct StatementIf
{
	Expression condition;
	Statement thenBranch;
	Statement elseBranch;
};

struct StatementWhile
{
	Expression condition;
	Statement body;
};

}