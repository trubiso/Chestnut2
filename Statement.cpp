#include "Statement.h"
#include "Specific.h"

namespace AST {

Parser::Parser<Statement> statement_create() {
	using namespace Parser;
	auto mutability = token_keyword(Token::Keyword::Let) >> constant(false) |
	                  token_keyword(Token::Keyword::Mut) >> constant(true);
	auto type_and_name = identifier_with_optional_type();
	auto value = expression();
	auto create_statement =
	    (mutability & type_and_name) & optional(token_symbol(Token::Symbol::Eq) >> value);

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

Parser::Parser<Statement> statement() {
	auto statement = statement_create();
	return Parser::trailing_semis() >> statement << Parser::eol();
}

Parser::Parser<std::vector<Statement>> scope() { return Parser::braced(Parser::many(statement())); }

} // namespace AST
