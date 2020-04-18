#pragma once

//STD
#include <string>
#include <variant>

namespace LoxCPP
{

template <typename T>
struct always_false
{
	inline static bool value = false;
};

struct None{};

struct Token
{
public:
	
	using Literal = std::variant<None, double, std::string>;

	enum class Type : std::uint8_t
	{
		ParenthesisLeft,
		ParenthesisRight,
		BraceLeft,
		BraceRight,
		Comma,
		Dot,
		Minus,
		Plus,
		Semicolon,
		Slash,
		Asterisk,

		Bang,
		BangEqual,
		Equal,
		EqualEqual,
		Greater,
		GreaterEqual,
		Less,
		LessEqual,

		Identifier,
		String,
		Number,

		And,
		Class,
		Else,
		False,
		Fun,
		For,
		If,
		Nil,
		Or,
		Print,
		Return,
		Super,
		This,
		True,
		Var,
		While,

		EndOfFile,
	};

	Type type;
	std::string lexeme;
	Literal literal;
	int line;
};

}