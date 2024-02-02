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
Parser<Token> token_keyword(Token::Keyword keyword);
Parser<Token> token_symbol(Token::Symbol symbol);

inline Parser<Token> wrapped(Parser<Token> const &parser, Token::Symbol l, Token::Symbol r) {
	return token_symbol(l) >> parser << token_symbol(r);
}
inline Parser<Token> parenthesized(Parser<Token> const &parser) {
	return wrapped(parser, Token::Symbol::LParen, Token::Symbol::RParen);
}
inline Parser<Token> bracketed(Parser<Token> const &parser) {
	return wrapped(parser, Token::Symbol::LBracket, Token::Symbol::RBracket);
}
inline Parser<Token> braced(Parser<Token> const &parser) {
	return wrapped(parser, Token::Symbol::LBrace, Token::Symbol::RBrace);
}
inline Parser<Token> angled(Parser<Token> const &parser) {
	return wrapped(parser, Token::Symbol::Lt, Token::Symbol::Gt);
}

inline Parser<Token> separated_by_comma(Parser<Token> const &parser) {
	return separated_by(parser, token_symbol(Token::Symbol::Comma));
}

} // namespace Parser
