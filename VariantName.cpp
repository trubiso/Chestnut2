#include "VariantName.h"

char const *get_variant_name(Token::Kind kind) {
	using enum Token::Kind;
	switch (kind) {
	case Identifier:
		return "identifier";
	case NumberLiteral:
		return "number literal";
	case StringLiteral:
		return "string literal";
	case CharLiteral:
		return "char literal";
	case Symbol:
		return "operator/punctuation";
	}
}

char const *get_variant_name(Token::Keyword keyword) {
	using enum Token::Keyword;
	switch (keyword) {
	case Let:
		return "let";
	case Mut:
		return "mut";
	case Pure:
		return "pure";
	case Func:
		return "func";
	case Method:
		return "method";
	case Struct:
		return "struct";
	case Enum:
		return "enum";
	case Union:
		return "union";
	}
}

char const *get_variant_name(Token::Symbol symbol) {
	using enum Token::Symbol;
	switch (symbol) {
	case Plus:
		return "+";
	case Minus:
		return "-";
	case Star:
		return "*";
	case Div:
		return "/";
	case Amp:
		return "&";
	case Bar:
		return "|";
	case Xor:
		return "^";
	case Tilde:
		return "~";
	case Eq:
		return "=";
	case Lt:
		return "<";
	case Gt:
		return ">";
	case Bang:
		return "!";
	case Question:
		return "?";
	case Percentage:
		return "%";
	case At:
		return "@";
	case LParen:
		return "(";
	case RParen:
		return ")";
	case LBracket:
		return "[";
	case RBracket:
		return "]";
	case LBrace:
		return "{";
	case RBrace:
		return "}";
	case Dot:
		return ".";
	case Comma:
		return ",";
	case Colon:
		return ":";
	case Semicolon:
		return ";";
	case DotDot:
		return "..";
	case DotDotDot:
		return "...";
	case ColonColon:
		return "::";
	case Arrow:
		return "->";
	case FatArrow:
		return "=>";
	}
}
