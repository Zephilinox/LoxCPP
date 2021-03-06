#pragma once

//STD
#include <memory>
#include <string>

//SELF
#include "Token.hpp"

namespace LoxCPP
{

struct ExpressionBinary;
struct ExpressionUnary;
struct ExpressionGrouping;
struct ExpressionAssignment;
struct ExpressionLogical;

struct ExpressionVariable
{
	Token name;
};

using Expression = std::variant<
	None,
	Token::Literal,
	ExpressionVariable,
	std::unique_ptr<ExpressionBinary>,
	std::unique_ptr<ExpressionUnary>,
	std::unique_ptr<ExpressionGrouping>,
	std::unique_ptr<ExpressionAssignment>,
	std::unique_ptr<ExpressionLogical>
>;

struct ExpressionBinary
{
	Expression left;
	Token operator_token;
	Expression right;
};

struct ExpressionUnary
{
	Token operator_token;
	Expression expression;
};

struct ExpressionGrouping
{
	Expression expression;
};

struct ExpressionAssignment
{
	Token name;
	Expression value;
};

struct ExpressionLogical
{
	Expression left;
	Token operator_token;
	Expression right;
};

inline std::string expressionToString(const Expression& expression)
{
	static auto visitor = [](const auto& expression) -> std::string {
		using Expr = typename std::decay_t<decltype(expression)>;
		
		if constexpr (std::is_same_v<Expr, std::unique_ptr<ExpressionUnary>>)
		{
			return "(" +
				   tokenToString(expression->operator_token) + " " + 
				   expressionToString(expression->expression) + ")";
		}
		else if constexpr (std::is_same_v<Expr, std::unique_ptr<ExpressionBinary>>)
		{
			return "(" +
				   tokenToString(expression->operator_token) + " " +
				   expressionToString(expression->left) + " " +
				   expressionToString(expression->right) + ")";
		}
		else if constexpr (std::is_same_v<Expr, Token::Literal>)
		{
			return tokenLiteralToString(expression);
		}
		else if constexpr (std::is_same_v<Expr, std::unique_ptr<ExpressionGrouping>>)
		{
			return "(group " + 
				expressionToString(expression->expression) + ")";
		}
		else
		{
			return "Invalid Expression";
		}
	};

	return std::visit<std::string>(visitor, expression);
}
}