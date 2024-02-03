#include "Function.h"
#include "Specific.h"

namespace AST {

Parser::Parser<Function::Signature> function_signature() {
	using namespace Parser;
	auto generics = optional(angled(separated_by_comma(identifier())));
	auto arguments = parenthesized(separated_by_comma(identifier_with_type()));
	auto return_type = type();
	return transform(generics & arguments & return_type, [](auto const &data) {
		return Function::Signature{
		    .generics = std::get<0>(std::get<0>(data)),
		    .arguments = std::get<1>(std::get<0>(data)),
		    .return_type = std::get<1>(data),
		};
	});
}

Parser::Parser<Function> function() {
	using namespace Parser;

	auto name = token_keyword(Token::Keyword::Func) >> identifier();

	// TODO: actually use body_shorthand (we need Statement::Return for it)
	auto body_shorthand = token_symbol(Token::Symbol::FatArrow) >> expression() << eol();
	auto body_scope = as_optional(scope());
	auto body_none = eol() >> constant((decltype(Function::body))std::nullopt);
	auto body = body_scope | body_none;

	return transform(name & function_signature() & body, [](auto const &data) {
		return Function{.name = std::get<0>(std::get<0>(data)),
		                .signature = std::get<1>(std::get<0>(data)),
		                .body = std::get<1>(data)};
	});
}

} // namespace AST