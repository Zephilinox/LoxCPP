#pragma once

//STD
#include <string>
#include <array>
#include <unordered_map>

//SELF
#include "LoxCPP/Token.hpp"
#include "LoxCPP/Interpreter.hpp"

class Runner
{
public:
	int run(std::string source);
	int runFile(char* file_name);
	int runPrompt();

	static void report(int line, std::string where, std::string message);
	
	static void error(int line, std::string message)
	{
		report(line, "", message);
	}

	static void error(LoxCPP::Token token, std::string message)
	{
		if (token.type == LoxCPP::Token::Type::EndOfFile)
			report(token.line, " at end", message);
		else
			report(token.line, " at '" + token.lexeme + "'", message);
	}

	inline static const std::unordered_map<std::string, LoxCPP::Token::Type> string_to_token_type{
		{"and", LoxCPP::Token::Type::And},
		{"class", LoxCPP::Token::Type::Class},
		{"else", LoxCPP::Token::Type::Else},
		{"false", LoxCPP::Token::Type::False},
		{"for", LoxCPP::Token::Type::For},
		{"fun", LoxCPP::Token::Type::Fun},
		{"if", LoxCPP::Token::Type::If},
		{"nil", LoxCPP::Token::Type::Nil},
		{"or", LoxCPP::Token::Type::Or},
		{"print", LoxCPP::Token::Type::Print},
		{"return", LoxCPP::Token::Type::Return},
		{"super", LoxCPP::Token::Type::Super},
		{"this", LoxCPP::Token::Type::This},
		{"true", LoxCPP::Token::Type::True},
		{"var", LoxCPP::Token::Type::Var},
		{"while", LoxCPP::Token::Type::While},
	};

	static void runtimeError(const LoxCPP::RuntimeError& error);

private:
	inline static bool hadError = false;
	inline static bool hadRuntimeError = false;
	static std::string toString(const LoxCPP::Token& token);
	static std::string toString(LoxCPP::Token::Type token_type);

	LoxCPP::Interpreter interpreter;
};