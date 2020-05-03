#include "Runner.hpp"

//STD
#include <iostream>
#include <fstream>
#include <sstream>

//SELF
#include "LoxCPP/Expressions.hpp"
#include "LoxCPP/Lexer.hpp"
#include "LoxCPP/Parser.hpp"

std::string Runner::toString(const LoxCPP::Token& token)
{
	return toString(token.type) +
		" '" + token.lexeme + "' " +
		tokenLiteralToString(token.literal) +
		" at line " + std::to_string(token.line);
}

std::string Runner::toString(const LoxCPP::Token::Type token_type)
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

void Runner::runtimeError(const LoxCPP::RuntimeError& error)
{
	std::cout << error.what() << "\n[line " << error.token.line << "]\n";
	hadRuntimeError = true;
}

int Runner::run(std::string source, bool print_expressions)
{
	LoxCPP::Lexer lexer(std::move(source));
	auto tokens = lexer.generateTokens();
	LoxCPP::Parser parser(std::move(tokens));
	std::vector<LoxCPP::Statement> statements = parser.parse();
	
	if (hadError)
		return -1;
	
	if (print_expressions)
	{
		if (statements.size() == 1 && 
			std::holds_alternative<LoxCPP::StatementExpression>(statements[0]))
		{
			auto expr_statement = std::move(std::get<LoxCPP::StatementExpression>(statements[0]));
			statements.clear();
			statements.emplace_back(LoxCPP::StatementPrint {
				std::move(expr_statement.expression)
			});
		}
	}
	
	interpreter.interpret(statements);
	
	return 0;
}

int Runner::runFile(char* file_name)
{
	std::ifstream file(file_name);
	if (file)
	{
		std::stringstream ss;
		ss << file.rdbuf();
		run(ss.str());

		if (hadError)
			return 65;
		
		if (hadRuntimeError)
			return 70;
	}

	return -1;
}

int Runner::runPrompt()
{	
	std::string line;
	while (true)
	{
		std::cout << "> ";
		std::getline(std::cin, line);
		
		if (line == ":q")
			break;
		
		run(line, true);
		hadError = false;
	}

	return 0;
}

void Runner::report(int line, std::string where, std::string message)
{
	std::cout << "[line " << line << "] Error" << where << ": " << message << "\n";
	hadError = true;
}
