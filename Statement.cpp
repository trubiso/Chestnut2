#include "Statement.h"
#include "Specific.h"

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

Parser::Parser<Statement> statement(bool semicolon) {
	auto statement = statement_create(semicolon);
	return statement;
}

Parser::Parser<std::vector<Statement>> scope() {
	return Parser::braced(Parser::trailing_semis() >> Parser::many(statement()));
}

} // namespace AST
