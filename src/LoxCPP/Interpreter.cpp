#include "Interpreter.hpp"

//STD
#include <iostream>

//SELF
#include "Runner.hpp"

namespace LoxCPP
{

Interpreter::Interpreter()
{
	//we always have a global environment
	environments.emplace_back();
	//todo: nasty hack to preserve pointers to environments, shouldn't ever need more than this :b
	//ideally we move to a proper stack, cba right now
	environments.reserve(10'000);
}

void Interpreter::interpret(const std::vector<Statement>& statements)
{	
	try
	{
		for (const auto& statement : statements)
		{
			execute(statement);
		}
	}
	catch (const RuntimeError& error)
	{
		Runner::runtimeError(error);
	}
}

void Interpreter::execute(const Statement& statement)
{
	handleStatement(statement);
}

void Interpreter::executeBlock(const std::vector<Statement>& statements)
{
	auto cur_env = &environments.back();
	environments.emplace_back();
	environments.back().parent = cur_env;

	try
	{
		for (const auto& statement : statements)
		{
			execute(statement);
		}
	}
	catch (std::exception& e)
	{
		//catch any exceptions so we guarantee we pop the environment
		//todo: replace with scoped guard
		environments.pop_back();
		throw;
	}

	environments.pop_back();
}

Token::Literal Interpreter::evaluate(const Expression& expression)
{
	const auto visitor = [this](const auto& expr) -> Token::Literal {
		using Expr = typename std::decay_t<decltype(expr)>;

		//todo: split out in to separate functions
		if constexpr (std::is_same_v<Expr, Token::Literal>)
		{
			return expr;
		}
		else if constexpr (std::is_same_v<Expr, std::unique_ptr<ExpressionGrouping>>)
		{
			return evaluate(expr->expression);
		}
		else if constexpr (std::is_same_v<Expr, std::unique_ptr<ExpressionUnary>>)
		{
			Token::Literal right = evaluate(expr->expression);

			switch (expr->operator_token.type)
			{
				case Token::Type::Minus:
				{
					if (!std::holds_alternative<double>(right))
						throw RuntimeError(expr->operator_token,
							"Operand must be a number");
					
					return -1 * std::get<double>(right);
				}
				case Token::Type::Bang:
				{
					return !isTruthy(right);
				}
				default:
				{
					throw RuntimeError(expr->operator_token, 
						"Invalid operator for Unary Expression");
				}
			}

			throw RuntimeError(expr->operator_token, 
				"Invalid operator for Unary Expression");
		}
		else if constexpr (std::is_same_v<Expr, std::unique_ptr<ExpressionBinary>>)
		{
			Token::Literal left = evaluate(expr->left);
			Token::Literal right = evaluate(expr->right);

			const bool left_is_double = std::holds_alternative<double>(left);
			const bool right_is_double = std::holds_alternative<double>(right);
			
			switch (expr->operator_token.type)
			{
				case Token::Type::Minus:
				{
					if (!left_is_double)
						throw RuntimeError(expr->operator_token, 
							"Left expression must evaluate to a number for subtraction");

					if (!right_is_double)
						throw RuntimeError(expr->operator_token,
							"Right expression must evaluate to a number for subtraction");

					return std::get<double>(left) - std::get<double>(right);
				}
				case Token::Type::Slash:
				{
					if (!left_is_double)
						throw RuntimeError(expr->operator_token, 
							"Left expression must evaluate to a number for division");

					if (!right_is_double)
						throw RuntimeError(expr->operator_token, 
							"Right expression must evaluate to a number for division");

					return std::get<double>(left) / std::get<double>(right);
				}
				case Token::Type::Asterisk:
				{
					if (!left_is_double)
						throw RuntimeError(expr->operator_token, 
							"Left expression must evaluate to a number for multiplication");

					if (!right_is_double)
						throw RuntimeError(expr->operator_token, 
							"Right expression must evaluate to a number for multiplication");


					return std::get<double>(left) * std::get<double>(right);
				}
				case Token::Type::Plus:
				{
					if (left_is_double && right_is_double)
						return std::get<double>(left) + std::get<double>(right);

					const bool left_is_string = std::holds_alternative<std::string>(left);
					const bool right_is_string = std::holds_alternative<std::string>(right);

					if (left_is_string && right_is_string)
						return std::get<std::string>(left) + std::get<std::string>(right);

					throw RuntimeError(expr->operator_token,
						"Both operands must be the same type, and must either be numbers or strings");
				}
				case Token::Type::Greater:
				{
					if (!left_is_double)
						throw RuntimeError(expr->operator_token,
							"Left expression must evaluate to a number for Greater equality");
					
					if (!right_is_double)
						throw RuntimeError(expr->operator_token,
							"Right expression must evaluate to a number for Greater equality");
					
					return std::get<double>(left) > std::get<double>(right);
				}
				case Token::Type::GreaterEqual:
				{
					if (!left_is_double)
						throw RuntimeError(expr->operator_token,
							"Left expression must evaluate to a number for GreaterEqual equality");
					
					if (!right_is_double)
						throw RuntimeError(expr->operator_token,
							"Right expression must evaluate to a number for GreaterEqual equality");

					return std::get<double>(left) >= std::get<double>(right);
				}
				case Token::Type::Less:
				{
					if (!left_is_double)
						throw RuntimeError(expr->operator_token,
							"Left expression must evaluate to a number for Less equality");
					
					if (!right_is_double)
						throw RuntimeError(expr->operator_token,
							"Right expression must evaluate to a number for Less equality");

					return std::get<double>(left) < std::get<double>(right);
				}
				case Token::Type::LessEqual:
				{
					if (!left_is_double)
						throw RuntimeError(expr->operator_token,
							"Left expression must evaluate to a number for LessEqual equality");
					
					if (!right_is_double)
						throw RuntimeError(expr->operator_token,
							"Right expression must evaluate to a number for LessEqual equality");

					return std::get<double>(left) <= std::get<double>(right);
				}
				case Token::Type::BangEqual:
				{
					return !isEqual(left, right);
				}
				case Token::Type::EqualEqual:
				{
					return isEqual(left, right);
				}
				default:
				{
					throw RuntimeError(expr->operator_token,
						"Invalid operator for Binary Expression");
				}
			}

			throw RuntimeError(expr->operator_token,
				"Invalid operator for Binary Expression");
		}
		else if constexpr (std::is_same_v<Expr, std::unique_ptr<ExpressionAssignment>>)
		{
			Token::Literal value = evaluate(expr->value);

			environments.back().assign(expr->name, value);
			return value;
		}
		else if constexpr (std::is_same_v<Expr, std::unique_ptr<ExpressionLogical>>)
		{
			Token::Literal left = evaluate(expr->left);

			if (expr->operator_token.type == Token::Type::Or)
			{
				if (isTruthy(left))
					return left;
			}
			else
			{
				if (!isTruthy(left))
					return left;
			}

			return evaluate(expr->right);
		}
		else if constexpr (std::is_same_v<Expr, ExpressionVariable>)
		{
			return environments.back().get(expr.name);
		}
		else if constexpr (std::is_same_v<Expr, None>)
		{
			//for some reason the expression evaluated to literally nothing
			//right now this is caused e.g. by a lone ';', not sure if this is correct
			//but it's a change that I made to fix variable assignment ending with ';', but maybe there's a better way
			return None{}; //"nil" which isn't exactly correct but good enough for now
		}
		else
		{
			static_assert(always_false<Expr>::value,
				"Unknown type. Did you forget to handle one of Expression?");
		}
	};
	
	auto value = std::visit<Token::Literal>(visitor, expression);

	if (std::holds_alternative<Uninitialized>(value))
	{
		throw RuntimeError({}, "Tried to evaluate an uninitialized value");
	}

	return value;
}

void Interpreter::handleStatement(const Statement& statement)
{
	const auto visitor = [this](const auto& statement) -> void {
		using Stmt = typename std::decay_t<decltype(statement)>;

		if constexpr (std::is_same_v<Stmt, StatementExpression>)
		{
			auto value = evaluate(statement.expression);
		}
		else if constexpr (std::is_same_v<Stmt, StatementPrint>)
		{
			auto literal = evaluate(statement.expression);
			std::cout << tokenLiteralToString(literal) << "\n";
		}
		else if constexpr (std::is_same_v<Stmt, StatementVariable>)
		{
			Token::Literal value;

			if (std::holds_alternative<Token::Literal>(statement.initializer))
			{
				value = std::get<Token::Literal>(statement.initializer);
			}
			else
			{
				value = evaluate(statement.initializer);
			}

			environments.back().define(statement.name.lexeme, value);
		}
		else if constexpr (std::is_same_v<Stmt, std::unique_ptr<StatementBlock>>)
		{
			executeBlock(statement->statements);
		}
		else if constexpr (std::is_same_v<Stmt, std::unique_ptr<StatementIf>>)
		{
			if (isTruthy(evaluate(statement->condition)))
				execute(statement->thenBranch);
			else if (!std::holds_alternative<None>(statement->elseBranch))
				execute(statement->elseBranch);
		}
		else if constexpr (std::is_same_v<Stmt, std::unique_ptr<StatementWhile>>)
		{
			while (isTruthy(evaluate(statement->condition)))
			{
				execute(statement->body);
			}
		}
		else if constexpr (std::is_same_v<Stmt, None>)
		{
			//todo?
			throw RuntimeError({}, "Invalid Statement");
		}
		else
		{
			static_assert(always_false<Stmt>::value,
				"Unknown type. Did you forget to handle one of Statement?");
		}
		
	};

	std::visit(visitor, statement);
}

bool Interpreter::isTruthy(const Token::Literal& literal)
{
	if (std::holds_alternative<None>(literal))
		return false;

	if (std::holds_alternative<bool>(literal))
		return std::get<bool>(literal);

	return true;
}
bool Interpreter::isEqual(const Token::Literal& left, const Token::Literal& right)
{
	if (std::holds_alternative<None>(left) && std::holds_alternative<None>(right))
		return true;
	
	if (std::holds_alternative<None>(left))
		return false;

	return left == right;
}
}
