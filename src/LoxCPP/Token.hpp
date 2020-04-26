#pragma once

//STD
#include <string>
#include <variant>
#include <string>

namespace LoxCPP
{

template <typename T>
struct always_false
{
	inline static constexpr bool value = false;
};

struct None{};

struct Token
{
public:
	
	using Literal = std::variant<None, bool, double, std::string>;

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

inline std::string tokenToString(const Token& token)
{
	return token.lexeme;
}

inline std::string tokenLiteralToString(const Token::Literal& token_literal)
{
	auto visitor = [](const auto& literal) -> std::string {
		using Lit = typename std::decay_t<decltype(literal)>;
		
		if constexpr (std::is_same_v<Lit, std::string>)
		{
			return literal;
		}
		else if constexpr (std::is_same_v<Lit, double>)
		{
			return std::to_string(literal);
		}
		else if constexpr (std::is_same_v<Lit, None>)
		{
			return "nil";
		}
		else if constexpr (std::is_same_v<Lit, bool>)
		{
			return literal ? "true" : "false";
		}
		else
		{
			static_assert(always_false<Lit>::value,
				"Unknown type. Did you forget to handle one of Token::Literal?");
		}
	};

	return std::visit<std::string>(visitor, token_literal);
}

}