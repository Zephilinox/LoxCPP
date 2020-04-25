#include "Interpreter.hpp"

//STD
#include <iostream>
#include <fstream>
#include <sstream>

//SELF
#include "LoxCPP/Expressions.hpp"
#include "LoxCPP/Lexer.hpp"

std::string Interpreter::toString(const LoxCPP::Token& token)
{
	return toString(token.type) +
		" '" + token.lexeme + "' " +
		tokenLiteralToString(token.literal) +
		" at line " + std::to_string(token.line);
}

std::string Interpreter::toString(const LoxCPP::Token::Type token_type)
{
#define TOKEN_TO_STRING_CASE(x) case LoxCPP::Token::Type::x: return #x;

	switch (token_type)
	{
		TOKEN_TO_STRING_CASE(ParenthesisLeft)
		TOKEN_TO_STRING_CASE(ParenthesisRight)
		TOKEN_TO_STRING_CASE(BraceLeft)
		TOKEN_TO_STRING_CASE(BraceRight)
		TOKEN_TO_STRING_CASE(Comma)
		TOKEN_TO_STRING_CASE(Dot)
		TOKEN_TO_STRING_CASE(Minus)
		TOKEN_TO_STRING_CASE(Plus)
		TOKEN_TO_STRING_CASE(Semicolon)
		TOKEN_TO_STRING_CASE(Slash)
		TOKEN_TO_STRING_CASE(Asterisk)
		
		TOKEN_TO_STRING_CASE(Bang)
		TOKEN_TO_STRING_CASE(BangEqual)
		TOKEN_TO_STRING_CASE(Equal)
		TOKEN_TO_STRING_CASE(EqualEqual)
		TOKEN_TO_STRING_CASE(Greater)
		TOKEN_TO_STRING_CASE(GreaterEqual)
		TOKEN_TO_STRING_CASE(Less)
		TOKEN_TO_STRING_CASE(LessEqual)
		
		TOKEN_TO_STRING_CASE(Identifier)
		TOKEN_TO_STRING_CASE(String)
		TOKEN_TO_STRING_CASE(Number)

		TOKEN_TO_STRING_CASE(And)
		TOKEN_TO_STRING_CASE(Class)
		TOKEN_TO_STRING_CASE(Else)
		TOKEN_TO_STRING_CASE(False)
		TOKEN_TO_STRING_CASE(Fun)
		TOKEN_TO_STRING_CASE(For)
		TOKEN_TO_STRING_CASE(If)
		TOKEN_TO_STRING_CASE(Nil)
		TOKEN_TO_STRING_CASE(Or)
		TOKEN_TO_STRING_CASE(Print)
		TOKEN_TO_STRING_CASE(Return)
		TOKEN_TO_STRING_CASE(Super)
		TOKEN_TO_STRING_CASE(This)
		TOKEN_TO_STRING_CASE(True)
		TOKEN_TO_STRING_CASE(Var)
		TOKEN_TO_STRING_CASE(While)
		
		TOKEN_TO_STRING_CASE(EndOfFile)
	}
#undef TOKEN_TO_STRING_CASE

	return "parser_error";
}

int Interpreter::run(std::string source)
{
	LoxCPP::Lexer lexer(source);
	std::vector<LoxCPP::Token> tokens = lexer.generateTokens();

	for (const auto token : tokens)
	{
		std::cout << toString(token) << "\n";
	}

	return 0;
}

int Interpreter::runFile(char* file_name)
{
	std::ifstream file(file_name);
	if (file)
	{
		std::stringstream ss;
		ss << file.rdbuf();
		run(ss.str());

		if (hadError)
			return 65;
	}

	return -1;
}

int Interpreter::runPrompt()
{
	std::unique_ptr<LoxCPP::ExpressionUnary> unary(new LoxCPP::ExpressionUnary {
		LoxCPP::Token{
			LoxCPP::Token::Type::Minus,
			"-",
			LoxCPP::None{},
			1,
		},
		123.0
	});
	
	std::unique_ptr<LoxCPP::ExpressionGrouping> group(new LoxCPP::ExpressionGrouping {
		45.67
	});

	std::unique_ptr<LoxCPP::ExpressionBinary> binary(new LoxCPP::ExpressionBinary{
		std::move(unary),
		 LoxCPP::Token{
			LoxCPP::Token::Type::Asterisk,
			"*",
			{},
			1,
		},
		std::move(group)
	});

	std::cout << expressionToString(LoxCPP::Expression(std::move(binary))) << "\n";
	
	return 0;
	
	std::string line;
	while (true)
	{
		std::cout << "> ";
		std::getline(std::cin, line);
		
		if (line == ":q")
			break;
		
		run(line);
		hadError = false;
	}

	return 0;
}

void Interpreter::report(int line, std::string where, std::string message)
{
	std::cout << "[line " << line << "] Error" << where << ": " << message << "\n";
	hadError = true;
}
