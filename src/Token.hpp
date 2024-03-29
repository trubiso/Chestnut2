#pragma once

#include <string>
#include <variant>

#include "Span.hpp"

struct Token {
	enum class Kind {
		Identifier,
		NumberLiteral,
		StringLiteral,
		CharLiteral,
		Symbol,
	};

	enum class Keyword {
		Const,
		Mut,
		Pure,
		Func,
		Method,
		Struct,
		Enum,
		Union,
	};

	enum class Symbol {
		Plus,  // +
		// TODO: PlusPlus (concat)
		Minus,       // -
		Star,        // *
		Div,         // /
		Amp,         // &
		Bar,         // |
		Xor,         // ^
		Tilde,       // ~
		Eq,          // =
		Lt,          // <
		Gt,          // >
		Bang,        // !
		Question,    // ?
		Percentage,  // %
		At,          // @
		LParen,      // (
		RParen,      // )
		LBracket,    // [
		RBracket,    // ]
		LBrace,      // {
		RBrace,      // }
		Dot,         // .
		Comma,       // ,
		Colon,       // :
		Semicolon,   // ;
		DotDot,      // ..
		DotDotDot,   // ...
		ColonColon,  // ::
		Arrow,       // ->
		FatArrow,    // =>
	};

	Span span;
	std::variant<std::string, Symbol> value;
	Kind kind;
};
