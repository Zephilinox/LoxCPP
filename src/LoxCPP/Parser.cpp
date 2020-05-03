#include "Parser.hpp"

//SELF
//todo: error handling should be part of the runner for this reason
#include "Runner.hpp"

namespace LoxCPP
{
Parser::Parser(std::vector<Token>&& tokens)
	: tokens(std::move(tokens))
{
}

std::vector<Statement> Parser::parse()
{
	std::vector<Statement> statements;

	while (!isAtEnd()) {
		statements.push_back(declaration());
	}

	return statements;
}

Expression Parser::expression()
{
	return assignment();
}

Expression Parser::equality()
{
	Expression expr = comparison();

	while (match(Token::Type::BangEqual, Token::Type::EqualEqual))
	{
		Token op = previous();
		Expression right = comparison();
		//todo: CPP20 allows brace initialized std::make_unique
		expr = std::unique_ptr<ExpressionBinary>(new ExpressionBinary {
			std::move(expr),
			std::move(op),
			std::move(right)
		});
	}

	return expr;
}

Expression Parser::comparison()
{
	Expression expr = addition();

	while (match(Token::Type::Greater, Token::Type::GreaterEqual, Token::Type::Less, Token::Type::LessEqual))
	{
		Token op = previous();
		Expression right = addition();
		//todo: CPP20 allows brace initialized std::make_unique
		expr = std::unique_ptr<ExpressionBinary>(new ExpressionBinary{
			std::move(expr),
			std::move(op),
			std::move(right)
		});
	}

	return expr;
}

Expression Parser::addition()
{
	Expression expr = multiplication();

	while (match(Token::Type::Minus, Token::Type::Plus))
	{
		Token op = previous();
		Expression right = multiplication();
		//todo: CPP20 allows brace initialized std::make_unique
		expr = std::unique_ptr<ExpressionBinary>(new ExpressionBinary{
			std::move(expr),
			std::move(op),
			std::move(right)});
	}

	return expr;
}

Expression Parser::multiplication()
{
	Expression expr = unary();

	while (match(Token::Type::Slash, Token::Type::Asterisk))
	{
		Token op = previous();
		Expression right = unary();
		//todo: CPP20 allows brace initialized std::make_unique
		expr = std::unique_ptr<ExpressionBinary>(new ExpressionBinary{
			std::move(expr),
			std::move(op),
			std::move(right)
		});
	}

	return expr;
}

Expression Parser::unary()
{
	if (match(Token::Type::Bang, Token::Type::Minus))
	{
		Token op = previous();
		Expression right = unary();
		//todo: CPP20 allows brace initialized std::make_unique
		return std::unique_ptr<ExpressionUnary>(new ExpressionUnary{
			std::move(op),
			std::move(right)
		});
	}

	return primary();
}

Expression Parser::primary()
{
	if (match(Token::Type::False))
		return false;

	if (match(Token::Type::True))
		return true;

	if (match(Token::Type::Nil))
		return None{};

	if (match(Token::Type::Number, Token::Type::String))
		return previous().literal;

	if (match(Token::Type::Identifier))
	{
		return ExpressionVariable{previous()};
	}
	
	if (match(Token::Type::ParenthesisLeft))
	{
		Expression expr = expression();
		consume(Token::Type::ParenthesisRight, "Expect ')' after expression.");
		//todo: CPP20 allows brace initialized std::make_unique
		return std::unique_ptr<ExpressionGrouping>(new ExpressionGrouping{
			std::move(expr)
		});
	}

	throw error(peek(), "Expect expression.");
}

std::vector<Statement> Parser::block()
{
	std::vector<Statement> statements;

	while (!check(Token::Type::BraceRight) && !isAtEnd())
		statements.push_back(declaration());

	consume(Token::Type::BraceRight, "Expect '}' after block.");
	return statements;
}

Statement Parser::statement()
{
	if (match(Token::Type::Print))
		return printStatement();

	if (match(Token::Type::BraceLeft))
		return std::unique_ptr<StatementBlock>(new StatementBlock {
			block()
		});

	if (match(Token::Type::If))
		return ifStatement();
	
	return expressionStatement();
}

Statement Parser::printStatement()
{
	Expression value = expression();
	consume(Token::Type::Semicolon, "Expect ';' after value");
	return StatementPrint{std::move(value)};
}

Statement Parser::expressionStatement()
{
	return StatementExpression{expression()};
}

Statement Parser::ifStatement()
{
	consume(Token::Type::ParenthesisLeft, "Expect '(' after 'if'.");
	Expression condition = expression();
	consume(Token::Type::ParenthesisRight, "Expect ')' after if condition.");

	Statement thenBranch = statement();
	Statement elseBranch = None{};

	if (match(Token::Type::Else))
		elseBranch = statement();

	return std::unique_ptr<StatementIf>(new StatementIf{
		std::move(condition),
		std::move(thenBranch),
		std::move(elseBranch)
	});
}

Statement Parser::declaration()
{
	try
	{
		if (match(Token::Type::Var))
			return variableDeclaration();

		return statement();
	}
	catch (std::exception& e)
	{
		synchronize();
		return None{};
	}
}

Expression Parser::assignment()
{
	Expression expr = equality();

	if (match(Token::Type::Equal))
	{
		Token equals = previous();
		Expression value = assignment();

		if (std::holds_alternative<ExpressionVariable>(expr))
		{
			Token name = std::get<ExpressionVariable>(expr).name;
			return std::unique_ptr<ExpressionAssignment>(new ExpressionAssignment{
				std::move(name), std::move(value)
			});
		}

		//discard exception object, just log, we don't need to synchronize
		error(std::move(equals), "Invalid assignment target.");
	}

	return expr;
}

Statement Parser::variableDeclaration()
{
	Token name = consume(Token::Type::Identifier, "Expect variable name.");

	Expression initializer = Uninitialized{};
	if (match(Token::Type::Equal))
	{
		initializer = expression();
	}

	consume(Token::Type::Semicolon, "Expect ';' after variable declaration");
	return StatementVariable{std::move(name), std::move(initializer)};
}

ParseError Parser::error(Token token, std::string message)
{
	Runner::error(std::move(token), std::move(message));
	return ParseError{};
}

void Parser::synchronize()
{
	advance();

	while (!isAtEnd())
	{
		if (previous().type == Token::Type::Semicolon)
			return;

		switch (peek().type)
		{
			case Token::Type::Class:
			case Token::Type::Fun:
			case Token::Type::Var:
			case Token::Type::For:
			case Token::Type::If:
			case Token::Type::While:
			case Token::Type::Print:
			case Token::Type::Return:
				return;
			default:
				advance();
		}
	}
}

}	 // namespace LoxCPP
