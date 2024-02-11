#include "Identifier.hpp"

#include "Specific.hpp"

namespace AST {

Parser::Parser<Identifier> identifier() {
	using namespace Parser;
	return transform(token_identifier(), [](Token const &identifier) {
		return Identifier{.kind = Identifier::Kind::Unqualified,
		                  .value = std::get<std::string>(identifier.value)};
	});
}

Parser::Parser<Identifier> identifier_qualified() {
	using namespace Parser;
	auto separator = token_symbol(Token::Symbol::ColonColon);
	auto inner_parser =
	    optional(separator) &
	    separated_no_trailing_at_least(
	        token_identifier(), separator, 1,
	        "empty qualified path ('::' was found where an identifier was expected)");
	return transform(
	           inner_parser,
	           [](std::tuple<std::optional<Token>, std::vector<Token>> const &data) -> Identifier {
		           std::optional<Token> const &absolute_token = std::get<0>(data);
		           bool absolute = absolute_token.has_value();
		           std::vector<Token> const &qualified_path_tokens = std::get<1>(data);
		           std::vector<std::string> qualified_path{};
		           for (Token const &token : qualified_path_tokens) {
			           qualified_path.push_back(std::get<std::string>(token.value));
		           }
		           // if we just received a regular identifier with no ::, it's just a regular
		           // identifier
		           if (qualified_path.size() == 1 && !absolute)
			           return Identifier{.kind = Identifier::Kind::Unqualified,
			                             .value = qualified_path.at(0)};
		           Identifier::QualifiedPath qualified_path_object{
		               .path = qualified_path,
		               .absolute = absolute,
		           };
		           return Identifier{.kind = Identifier::Kind::Qualified,
		                             .value = qualified_path_object};
	           }) |
	       identifier();
}

}  // namespace AST
