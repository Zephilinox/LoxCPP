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

struct Uninitialized
{
	friend constexpr bool operator==(const Uninitialized& left, const Uninitialized& right);
};

constexpr bool operator==([[maybe_unused]] const Uninitialized& left, [[maybe_unused]] const Uninitialized& right)
{
	return false;
}

struct None
{
	friend constexpr bool operator==(const None& left, const None& right);
};

constexpr bool operator==([[maybe_unused]] const None& left, [[maybe_unused]] const None& right)
{
	return true;
}

struct Token
{
public:
	
	using Literal = std::variant<None, Uninitialized, bool, double, std::string>;

	enum class Type : std::uint8_t
	{
		Invalid,
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

	Type type = Type::Invalid;
	std::string lexeme;
	Literal literal;
	int line = -1;
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
		else if constexpr (std::is_same_v<Lit, Uninitialized>)
		{
			//this should never occur, but don't want to throw an exception here for now
			return "internal compiler error. uninitialized value evaluated";
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