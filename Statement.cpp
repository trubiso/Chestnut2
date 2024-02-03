#include "Statement.h"
#include "Specific.h"

namespace AST {

Parser::Parser<Statement> statement_create(bool semicolon) {
	using namespace Parser;
	auto mutability = token_keyword(Token::Keyword::Let) >> constant(false) |
	                  token_keyword(Token::Keyword::Mut) >> constant(true);
	auto type_and_name = must(identifier_with_optional_type());
	auto value = expression();
	auto create_statement = (mutability & type_and_name) &
	                        must(as_optional(token_symbol(Token::Symbol::Eq) >> value << eol(semicolon)) |
	                             eol(semicolon) >> constant((std::optional<Expression>)std::nullopt));

	return transform(create_statement,
	                 [](std::tuple<std::tuple<bool, std::tuple<Identifier, std::optional<Type>>>,
	                               std::optional<Expression>> const &data) {
		                 std::tuple<bool, std::tuple<Identifier, std::optional<Type>>> const &lhs =
		                     std::get<0>(data);
		                 bool mutable_ = std::get<0>(lhs);
		                 std::tuple<Identifier, std::optional<Type>> const &type_and_name =
		                     std::get<1>(lhs);
		                 Identifier identifier = std::get<0>(type_and_name);
		                 Type type = std::get<1>(type_and_name).value_or(Type::inferred());
		                 std::optional<Expression> value = std::get<1>(data);
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
