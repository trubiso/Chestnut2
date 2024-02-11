#include "Function.hpp"
#include "Specific.hpp"

namespace AST {

Parser::Parser<Function::Signature> function_signature() {
	using namespace Parser;
	auto generics = optional(spanned(angled(separated_by_comma(spanned(identifier())))));
	auto arguments = spanned(parenthesized(separated_by_comma(spanned(identifier_with_type()))));
	auto return_type = spanned(type());
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

	auto name = token_keyword(Token::Keyword::Func) >> spanned(must(identifier()));

	// TODO: actually use body_shorthand (we need Statement::Return for it)
	// auto body_shorthand = token_symbol(Token::Symbol::FatArrow) >> expression() << eol();
	auto body_scope = as_optional(spanned(scope()));
	auto body_none = eol() >> constant((decltype(Function::body))std::nullopt);
	auto body = body_scope | body_none;

	return transform(name & spanned(must(function_signature())) & must(body), [](auto const &data) {
		return Function{.name = std::get<0>(std::get<0>(data)),
		                .signature = std::get<1>(std::get<0>(data)),
		                .body = std::get<1>(data)};
	});
}

} // namespace AST