#include "Parser.hpp"

//SELF
#include "Interpreter.hpp"

namespace LoxCPP
{
Parser::Parser(std::vector<Token>&& tokens)
	: tokens(std::move(tokens))
{
}

Expression Parser::parse()
{
	try
	{
		return expression();
	}
	catch ([[maybe_unused]] const ParseError& error)
	{
		return None{};
	}
}

Expression Parser::expression()
{
	return equality();
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

ParseError Parser::error(Token token, std::string message)
{
	Interpreter::error(std::move(token), std::move(message));
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
