#include "Expression.hpp"
#include "Combinator.hpp"
#include "Primitive.hpp"
#include "Specific.hpp"

namespace AST {

#define PARSER_LITERAL(T)                                                                          \
	::Parser::transform(Parser::token_kind(Token::Kind::T), [](Token const &token) {               \
		return Expression{.kind = Expression::Kind::T,                                             \
		                  .value = std::get<std::string>(token.value)};                            \
	})

Parser::Parser<Expression> expression_char_literal() { return PARSER_LITERAL(CharLiteral); }
Parser::Parser<Expression> expression_string_literal() { return PARSER_LITERAL(StringLiteral); }
Parser::Parser<Expression> expression_number_literal() { return PARSER_LITERAL(NumberLiteral); }
Parser::Parser<Expression> expression_identifier() {
	return Parser::transform(identifier_qualified(), [](Identifier const &identifier) {
		return Expression{.kind = Expression::Kind::Identifier, .value = identifier};
	});
}

Parser::Parser<Expression> expression_atom() {
	return expression_identifier() | expression_number_literal() | expression_string_literal() |
	       expression_char_literal() | lazy(Parser::parenthesized(expression()));
}

#define BINARY_OPERATOR(OP, NEXT)                                                                  \
	transform(spanned(NEXT) & many(spanned(OP) & spanned(NEXT)), [](auto const &data) {            \
		Spanned<Expression> lhs = std::get<0>(data);                                               \
		for (auto const &pair : std::get<1>(data)) {                                               \
			auto const &operator_ = std::get<0>(pair);                                             \
			auto const &rhs = std::get<1>(pair);                                                   \
			Expression::BinaryOperation operation{.lhs = lhs, .rhs = rhs, .operator_ = operator_}; \
			lhs =                                                                                  \
			    Spanned<Expression>{.value = Expression{.kind = Expression::Kind::BinaryOperation, \
			                                            .value = operation},                       \
			                        .span = Span(lhs.span.start, rhs.span.end)};                   \
		}                                                                                          \
		return lhs.value;                                                                          \
	})

#define UNARY_OPERATOR(OP, NEXT)                                                                   \
	transform(many(spanned(OP)) & spanned(NEXT), [](auto const &data) {                            \
		Spanned<Expression> rhs = std::get<1>(data);                                               \
		for (auto const &operator_ : std::get<0>(data)) {                                          \
			Expression::UnaryOperation operation{.value = rhs, .operator_ = operator_};            \
			rhs = Spanned<Expression>{                                                             \
			    .value = Expression{.kind = Expression::Kind::UnaryOperation, .value = operation}, \
			    .span = Span(operator_.span.start, rhs.span.end)};                                 \
		}                                                                                          \
		return rhs.value;                                                                          \
	})

#define OPERATOR(OP) (token_symbol(OP) >> constant(OP))

Parser::Parser<Expression> expression() {
	using namespace Parser;
	using enum Token::Symbol;
	auto minus = OPERATOR(Minus);
	auto plus_minus = OPERATOR(Plus) | OPERATOR(Minus);
	auto star_div = OPERATOR(Star) | OPERATOR(Div);

	auto unary_minus = UNARY_OPERATOR(minus, expression_atom());
	auto binary_star_div = BINARY_OPERATOR(star_div, unary_minus);
	auto binary_plus_minus = BINARY_OPERATOR(plus_minus, binary_star_div);

	return binary_plus_minus;
}

} // namespace AST
