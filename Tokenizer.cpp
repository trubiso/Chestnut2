#include "Tokenizer.h"

#include <cctype>
#include <tuple>

std::optional<std::tuple<Token, std::optional<Token>>> Tokenizer::next() {
	consume_whitespace();
	if (!current().has_value())
		return {};
	char current_value = current().value();
	bool should_attempt_semicolon = false;
	while (current_value == '\n') {
		should_attempt_semicolon = true;
		advance();
		consume_whitespace();
		if (!current().has_value())
			return {};
		current_value = current().value();
	}

	size_t begin = m_index;
	Token::Kind kind;
	Token::Symbol symbol;

	if (std::isalpha(current_value)) {
		kind = Token::Kind::Identifier;
		consume_identifier();
	} else if (std::isdigit(current_value)) {
		kind = Token::Kind::NumberLiteral;
		consume_number_literal();
	} else
		switch (current_value) {
		case '"':
			kind = Token::Kind::StringLiteral;
			consume_wrapped_literal('"');
			break;
		case '\'':
			kind = Token::Kind::CharLiteral;
			consume_wrapped_literal('\'');
			break;
		case '+':
		case '-':
		case '*':
		case '/':
		case '&':
		case '|':
		case '^':
		case '~':
		case '=':
		case '<':
		case '>':
		case '!':
		case '?':
		case '%':
		case '@':
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '.':
		case ',':
		case ':':
		case ';': {
			kind = Token::Kind::Symbol;
			symbol = consume_symbol();
			break;
		}
		default:
			// unknown char, how did we get here?
			return {};
		}

	std::optional<Token> semicolon = {};
	if (should_attempt_semicolon) {
		bool insert = false;
		if (kind == Token::Kind::Identifier)
			insert = true;
		if (kind == Token::Kind::Symbol)
			switch (symbol) {
			case Token::Symbol::Plus:
			case Token::Symbol::Minus:
			case Token::Symbol::Star:
			case Token::Symbol::Div:
			case Token::Symbol::Amp:
			case Token::Symbol::Bar:
			case Token::Symbol::Xor:
			case Token::Symbol::Tilde:
			case Token::Symbol::Eq:
			case Token::Symbol::Lt:
			case Token::Symbol::Gt:
			case Token::Symbol::Bang:
			case Token::Symbol::Question:
			case Token::Symbol::Percentage:
			case Token::Symbol::At:
			case Token::Symbol::Dot:
			case Token::Symbol::Comma:
			case Token::Symbol::Colon:
			case Token::Symbol::Semicolon:
			case Token::Symbol::Arrow:
			case Token::Symbol::FatArrow:
				insert = false;
				break;
			case Token::Symbol::LParen:
			case Token::Symbol::RParen:
			case Token::Symbol::LBracket:
			case Token::Symbol::RBracket:
			case Token::Symbol::LBrace:
			case Token::Symbol::RBrace:
			case Token::Symbol::ColonColon:
			case Token::Symbol::DotDot:
			case Token::Symbol::DotDotDot:
				insert = true;
				break;
			}
		if (insert) {
			semicolon = Token{.span = Span{begin, begin + 1},
			                  .value = Token::Symbol::Semicolon,
			                  .kind = Token::Kind::Symbol};
		}
	}

	size_t end = m_index;
	Span span{begin, end};
	if (kind != Token::Kind::Symbol)
		return std::tuple<Token, std::optional<Token>>{
		    Token{.span = span, .value = span.value(*m_source), .kind = kind}, semicolon};
	else {
		return std::tuple<Token, std::optional<Token>>{
		    Token{.span = span, .value = symbol, .kind = kind}, semicolon};
	}
}

void Tokenizer::consume_whitespace() {
	while (is_index_valid() && is_whitespace(current().value())) {
		advance();
	}
}

void Tokenizer::consume_identifier() {
	while (is_index_valid() && std::isalnum(current().value())) {
		advance();
	}
}

void Tokenizer::consume_number_literal() {
	// TODO: be stricter
	while (is_index_valid() && (std::isdigit(current().value()) || current().value() == '_'))
		advance();
	if (is_index_valid()) {
		switch (current().value()) {
		case 'x':
		case 'b':
		case 'o':
			advance(); // consume the base character
			break;
		case '.':
			advance(); // consume the decimal point (0x, 0b, 0o are integer only)
			break;
		}
	}
	while (is_index_valid() && (std::isxdigit(current().value()) || current().value() == '_'))
		advance();
}

void Tokenizer::consume_wrapped_literal(char wrap) {
	advance(); // consume wrap character
	while (is_index_valid() && current().value() != wrap)
		advance();
	if (!is_index_valid()) {
		// unclosed wrapped literal diagnostic!
		return;
	}
	advance(); // consume wrap character
}

Token::Symbol from_char(char c) {
	using enum Token::Symbol;
	switch (c) {
	case '+':
		return Plus;
	case '-':
		return Minus;
	case '*':
		return Star;
	case '/':
		return Div;
	case '&':
		return Amp;
	case '|':
		return Bar;
	case '^':
		return Xor;
	case '~':
		return Tilde;
	case '=':
		return Eq;
	case '<':
		return Lt;
	case '>':
		return Gt;
	case '!':
		return Bang;
	case '?':
		return Question;
	case '%':
		return Percentage;
	case '@':
		return At;
	case '(':
		return LParen;
	case ')':
		return RParen;
	case '[':
		return LBracket;
	case ']':
		return RBracket;
	case '{':
		return LBrace;
	case '}':
		return RBrace;
	case '.':
		return Dot;
	case ',':
		return Comma;
	case ':':
		return Colon;
	case ';':
		return Semicolon;
	default:
		[[assume(false)]];
	}
}

Token::Symbol Tokenizer::consume_symbol() {
	char first_char = current().value();
	Token::Symbol symbol = from_char(first_char);
	advance();
	if (is_index_valid()) {
		switch (current().value()) {
		case '>':
			if (first_char == '-') {
				symbol = Token::Symbol::Arrow;
				advance();
			} else if (first_char == '=') {
				symbol = Token::Symbol::FatArrow;
				advance();
			}
			break;
		case ':':
			if (first_char == ':') {
				symbol = Token::Symbol::ColonColon;
				advance();
			}
			break;
		case '.':
			if (first_char == '.') {
				symbol = Token::Symbol::DotDot;
				advance();
			}
		}
	}
	if (is_index_valid()) {
		if (current().value() == '.' && symbol == Token::Symbol::DotDot) {
			symbol = Token::Symbol::DotDotDot;
			advance();
		}
	}
	return symbol;
}
