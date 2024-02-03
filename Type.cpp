#include "Type.h"
#include "Specific.h"

namespace AST {

Parser::Parser<Type> type_identified() {
	using namespace Parser;
	return transform(identifier_qualified(), [](Identifier const &identifier) {
		return Type{.kind = Type::Kind::Identified, .value = identifier};
	});
}

Parser::Parser<Type> type_inferred() {
	using namespace Parser;
	return filter(identifier_qualified(),
	              [](Identifier const &identifier) -> std::optional<std::string> {
		              if (identifier.kind != Identifier::Kind::Unqualified ||
		                  std::get<std::string>(identifier.value) != "_")
			              return "expected discarded type";
		              return {};
	              }) >>
	       constant(Type::inferred());
}

Parser::Parser<Type> type() { return type_inferred() | type_identified(); }

Parser::Parser<std::tuple<Identifier, Type>> identifier_with_type() {
	return identifier() & (Parser::token_symbol(Token::Symbol::Colon) >> type());
}

Parser::Parser<std::tuple<Identifier, std::optional<Type>>> identifier_with_optional_type() {
	return identifier() & optional(Parser::token_symbol(Token::Symbol::Colon) >> type());
}

} // namespace AST
