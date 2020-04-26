#include "Lexer.hpp"

//STD
#include <cassert>
#include <iostream>

//SELF
#include "Runner.hpp"

namespace LoxCPP
{

Lexer::Lexer(std::string source)
	: source(source)
{
	std::cout << "Input: " << source << "\n";
}

std::vector<LoxCPP::Token> Lexer::generateTokens()
{
	while (!isAtEnd())
	{
		start = current;
		scanToken();
	}

	tokens.push_back({Token::Type::EndOfFile, "", {}, line});
	
	return tokens;
}

char Lexer::advance()
{
	current++;
	assert(current > 0);
	return source[static_cast<std::size_t>(current) - 1];
}

void Lexer::scanToken()
{
	//std::cout << "scanning at " << current << "\n";
	//peek();
	char c = advance();
	//std::cout << "c is " << c << "\n";
	switch (c)
	{
		case '(': addToken(Token::Type::ParenthesisLeft); break;
		case ')': addToken(Token::Type::ParenthesisRight); break;
		case '{': addToken(Token::Type::BraceLeft); break;
		case '}': addToken(Token::Type::BraceRight); break;
		case ',': addToken(Token::Type::Comma); break;
		case '.': addToken(Token::Type::Dot); break;
		case '-': addToken(Token::Type::Minus); break;
		case '+': addToken(Token::Type::Plus); break;
		case ';': addToken(Token::Type::Semicolon); break;
		case '*': addToken(Token::Type::Asterisk); break;
		case '!': addToken(match('=') ? Token::Type::BangEqual : Token::Type::Bang); break;
		case '=': addToken(match('=') ? Token::Type::EqualEqual : Token::Type::Equal); break;
		case '<': addToken(match('=') ? Token::Type::LessEqual : Token::Type::Less); break;
		case '>': addToken(match('=') ? Token::Type::GreaterEqual : Token::Type::Greater); break;
		case '/':
		{
			if (match('/'))
				while (peek() != '\n' && !isAtEnd())
					advance();
			else
				addToken(Token::Type::Slash);
		}
		case ' ':
		case '\r':
		case '\t':
			break;
		case '\n':
			line++;
		case '"': handleStringCharacter(); break;
		default:
		{
			if (isDigit(c))
			{
				handleNumberCharacter();
			}
			else if (isAlpha(c))
			{
				handleIdentifierCharacter();
			}
			else
			{
				Runner::error(line, std::string("Unexpected character: ") + c);
			}
		}
	}
}

void Lexer::addToken(Token::Type type)
{
	addToken(type, {});
}

void Lexer::addToken(Token::Type type, Token::Literal literal)
{
	assert(current >= 0);
	assert(start >= 0);
	assert(current <= static_cast<int>(source.length()));
	
	std::string text = source.substr(start, static_cast<std::size_t>(current) - start);
	tokens.push_back({type, std::move(text), std::move(literal), line});
}

char Lexer::peek(int offset)
{
	assert(current + offset <= static_cast<int>(source.length()));
	assert(current >= 0);
	assert(current + offset >= 0);
	return source[static_cast<std::size_t>(current) + offset];
}

bool Lexer::match(char expected)
{
	if (isAtEnd())
		return false;
	
	if (peek() != expected)
		return false;

	current++;
	return true;
}

void Lexer::handleStringCharacter()
{
	int start_line = line;

	//Keep going until we find the matching "
	while (peek() != '"' && !isAtEnd())
	{
		if (peek() == '\n')
			line++;

		advance();
	}

	if (isAtEnd())
	{
		Runner::error(start_line, "Unterminated string");
		return;
	}

	//skip over the last '"'
	advance();

	//Everything between the quotes
	assert(start + 1 < static_cast<int>(source.size()));
	assert(current <= static_cast<int>(source.size()));
	assert(start + 1 < current - 1);
	std::string literal = source.substr(
		static_cast<std::size_t>(start) + 1,
		static_cast<std::size_t>(current) - 1);
	addToken(Token::Type::String, literal);
}

void Lexer::handleNumberCharacter()
{
	while (isDigit(peek()))
		advance();

	if (peek() == '.')
	{
		if (isDigit(peek(1)))
		{
			advance();

			while (isDigit(peek()))
				advance();
		}
		else
		{
			Runner::error(line, "Expected digit after '.' when parsing number " +
				source.substr(start, current) + " at line " + std::to_string(line) +
				" but found '" + peek(1) + "' instead.");
		}
	}

	addToken(Token::Type::Number, std::stod(source.substr(start, current)));
}

void Lexer::handleIdentifierCharacter()
{
	while (isAlphaNumeric(peek()))
		advance();

	std::string text = source.substr(start, current);

	const auto is_reserved = Runner::string_to_token_type.contains(text);
	if (is_reserved)
		addToken(Runner::string_to_token_type.at(text));
	else
		addToken(Token::Type::Identifier);	
}
}
