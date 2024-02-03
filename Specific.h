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
	return filter(token_identifier(), [=](Token const &token) {
		return std::get<std::string>(token.value) == kw ? std::optional<std::string>(std::nullopt)
		                                                : ("expected keyword " + kw);
	});
}
inline Parser<Token> token_symbol(Token::Symbol symbol) {
	return filter(token_kind(Token::Kind::Symbol), [=](Token const &token) {
		return std::get<Token::Symbol>(token.value) == symbol
		           ? std::optional<std::string>(std::nullopt)
		           : ("expected symbol " + (std::string)get_variant_name(symbol));
	});
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
inline Parser<std::vector<T>, E, V> separated_by_comma(Parser<T, E, V> const &parser) {
	return separated(parser, token_symbol(Token::Symbol::Comma));
}

} // namespace Parser
