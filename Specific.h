#pragma once
#include "Combinator.h"
#include "Primitive.h"
#include "Token.h"
#include "VariantName.h"

namespace Parser {

inline Parser<Token> token_kind(Token::Kind kind) {
	return satisfy([=](Token const &token) { return token.kind == kind; },
	               "expected token of kind " + std::string(get_variant_name(kind)));
}
inline Parser<Token> token_identifier() { return token_kind(Token::Kind::Identifier); }
inline Parser<Token> token_keyword(Token::Keyword keyword) {
	std::string kw = get_variant_name(keyword);
	return satisfy(
	    [=](Token const &token) {
		    return token.kind == Token::Kind::Identifier &&
		           std::get<std::string>(token.value) == kw;
	    },
	    "expected keyword " + kw);
}
inline Parser<Token> token_symbol(Token::Symbol symbol) {
	return satisfy(
	    [=](Token const &token) {
		    return token.kind == Token::Kind::Symbol &&
		           std::get<Token::Symbol>(token.value) == symbol;
	    },
	    "expected symbol " + (std::string)get_variant_name(symbol));
}

template <typename T, typename E, typename V>
inline Parser<T, E, V> wrapped(Parser<T, E, V> const &parser, Token::Symbol l, Token::Symbol r) {
	return token_symbol(l) >> parser << token_symbol(r);
}
template <typename T, typename E, typename V>
inline Parser<T, E, V> parenthesized(Parser<T, E, V> const &parser) {
	return wrapped(parser, Token::Symbol::LParen, Token::Symbol::RParen);
}
template <typename T, typename E, typename V>
inline Parser<T, E, V> bracketed(Parser<T, E, V> const &parser) {
	return wrapped(parser, Token::Symbol::LBracket, Token::Symbol::RBracket);
}
template <typename T, typename E, typename V>
inline Parser<T, E, V> braced(Parser<T, E, V> const &parser) {
	return wrapped(parser, Token::Symbol::LBrace, Token::Symbol::RBrace);
}
template <typename T, typename E, typename V>
inline Parser<T, E, V> angled(Parser<T, E, V> const &parser) {
	return wrapped(parser, Token::Symbol::Lt, Token::Symbol::Gt);
}
template <typename T, typename E, typename V>
inline Parser<std::vector<T>, E, V> separated_by_comma(Parser<T, E, V> const &parser,
                                                       bool can_be_empty = true) {
	auto actual_parser = separated(parser, token_symbol(Token::Symbol::Comma));
	if (can_be_empty)
		return actual_parser | constant(std::vector<T>{});
	else
		return actual_parser;
}

inline Parser<std::vector<Token>> trailing_semis() {
	return many(token_symbol(Token::Symbol::Semicolon));
}

inline Parser<std::vector<Token>> eol(bool semicolon = true) {
	return at_least(token_symbol(Token::Symbol::Semicolon), semicolon ? 1 : 0,
	                "expected semicolon or newline");
}

} // namespace Parser
