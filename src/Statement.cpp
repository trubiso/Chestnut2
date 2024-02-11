#include "Statement.hpp"
#include "Specific.hpp"

namespace AST {

Parser::Parser<Statement> statement_create(bool semicolon) {
	using namespace Parser;
	auto mutability = spanned(token_keyword(Token::Keyword::Let) >> constant(false) |
	                          token_keyword(Token::Keyword::Mut) >> constant(true));
	auto type_and_name = must(identifier_with_optional_type());
	auto value = spanned(expression());
	auto create_statement =
	    (mutability & type_and_name) &
	    must(as_optional(token_symbol(Token::Symbol::Eq) >> value << eol(semicolon)) |
	         eol(semicolon) >> constant((std::optional<Spanned<Expression>>)std::nullopt));

	return transform(create_statement, [](auto const &data) {
		auto const &lhs = std::get<0>(data);
		Spanned<bool> mutable_ = std::get<0>(lhs);
		auto const &type_and_name = std::get<1>(lhs);
		Spanned<Identifier> identifier = std::get<0>(type_and_name);
		Spanned<Type> type =
		    std::get<1>(type_and_name)
		        .value_or(Spanned<Type>{.value = Type::inferred(), .span = mutable_.span});
		std::optional<Spanned<Expression>> value = std::get<1>(data);
		Statement::Create create{
		    .type = type,
		    .identifier = identifier,
		    .mutable_ = mutable_,
		    .value = value,
		};
		return Statement{.kind = Statement::Kind::Create, .value = create};
	});
}

Parser::Parser<Statement> statement_set(bool semicolon) {
	using namespace Parser;
	// TODO: binary_operator() (just a filter() on token_symbol(), or a validate())
	auto lhs = spanned(expression());
	auto operator_ = spanned(
	    optional(spanned(filter(token_kind(Token::Kind::Symbol),
	                            [](Token const &token) -> std::optional<std::string> {
		                            // avoid == being a thing in set stmt
		                            if (std::get<Token::Symbol>(token.value) == Token::Symbol::Eq)
			                            return "";
		                            else
			                            return {};
	                            })))
	    << token_symbol(Token::Symbol::Eq));
	auto rhs = spanned(expression());
	auto set_statement = lhs & operator_ & must(rhs) << eol(semicolon);
	return transform(set_statement, [](auto const &data) {
		auto const &lhs = std::get<0>(std::get<0>(data));
		auto const &operator_ = std::get<1>(std::get<0>(data));
		auto const &rhs = std::get<1>(data);
		if (bool(operator_.value)) {
			Expression actual_rhs = Expression{
			    .kind = Expression::Kind::BinaryOperation,
			    .value = Expression::BinaryOperation{
			        .lhs = lhs,
			        .rhs = rhs,
			        .operator_ = Spanned<Token::Symbol>{
			            .value = std::get<Token::Symbol>(operator_.value.value().value.value),
			            .span = operator_.value.value().span,
			        }}};
			// TODO: fix up the span here
			return Statement{.kind = Statement::Kind::Set,
			                 .value = Statement::Set{
			                     .lhs = lhs,
			                     .rhs = Spanned<Expression>{.value = actual_rhs, .span = rhs.span},
			                     .operator_span = operator_.span}};
		} else {
			return Statement{
			    .kind = Statement::Kind::Set,
			    .value = Statement::Set{.lhs = lhs, .rhs = rhs, .operator_span = operator_.span}};
		}
	});
}

Parser::Parser<Statement> statement_bare_expression(bool semicolon) {
	using namespace Parser;
	auto bare_expression_statement = spanned(expression()) << eol(semicolon);
	return transform(bare_expression_statement, [](auto const &data) {
		return Statement{.kind = Statement::Kind::BareExpression,
		                 .value = Statement::BareExpression{.expression = data}};
	});
}

Parser::Parser<Statement> statement(bool semicolon) {
	auto statement = statement_create(semicolon) | statement_set(semicolon) |
	                 statement_bare_expression(semicolon);
	return statement;
}

Parser::Parser<std::vector<Statement>> scope() {
	return Parser::braced(Parser::trailing_semis() >> Parser::many(statement()));
}

} // namespace AST
