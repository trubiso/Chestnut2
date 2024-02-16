#include "Type.hpp"

#include <cctype>

#include "Specific.hpp"

namespace AST {

Parser::Parser<Type> type_identified() {
	using namespace Parser;
	return transform(identifier_qualified(), [](Identifier const &identifier) {
		return Type{.kind = Type::Kind::Identified, .value = identifier};
	});
}

#define BUILT_IN(N, EV)                                      \
	if (name == N) return Type::BuiltIn {                    \
			.kind = Type::BuiltIn::Kind::EV, .bit_width = {} \
		}

Parser::Parser<Type> type_built_in() {
	using namespace Parser;
	auto mapped =
	    transform(identifier(), [](Identifier const &identifier) -> std::optional<Type::BuiltIn> {
		    auto name = std::get<std::string>(identifier.value);
		    BUILT_IN("char", Char);
		    BUILT_IN("void", Void);
		    // TODO: strings
		    Type::BuiltIn::Kind kind = Type::BuiltIn::Kind::Void;
		    uint32_t width = 0;
		    // this loop should never get skipped
		    for (size_t i = 0; i < name.size(); ++i) {
			    auto const &curr = name.at(i);
			    if (i == 0) switch (curr) {
				    case 'i':
					    kind = Type::BuiltIn::Kind::Int;
					    break;
				    case 'u':
					    kind = Type::BuiltIn::Kind::Uint;
					    break;
				    case 'f':
					    kind = Type::BuiltIn::Kind::Float;
					    break;
				    default:
					    return {};
				    }
			    else {
				    if (!std::isdigit(curr)) return {};
				    uint8_t digit = curr - '0';
				    uint32_t begin_width = width;
				    width *= 10;
				    width += digit;
				    if (width <= begin_width) return {};  // this accounts for i0 and i2147483648
			    }
		    }
		    return Type::BuiltIn{.kind = kind, .bit_width = width};
	    });
	auto filtered = filter(
	    mapped, [](std::optional<Type::BuiltIn> const &built_in) -> std::optional<std::string> {
		    if (built_in.has_value())
			    return {};
		    else
			    return "invalid built-in type";
	    });
	return transform(filtered, [](std::optional<Type::BuiltIn> const &built_in) {
		// we know built_in holds a value
		return Type{.kind = Type::Kind::BuiltIn, .value = built_in.value()};
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

Parser::Parser<Type> type() { return type_inferred() | type_built_in() | type_identified(); }

Parser::Parser<std::tuple<Spanned<Identifier>, Spanned<Type>>> identifier_with_type() {
	return Parser::spanned(identifier()) &
	       (Parser::token_symbol(Token::Symbol::Colon) >> Parser::spanned(type()));
}

Parser::Parser<std::tuple<Spanned<Identifier>, std::optional<Spanned<Type>>>>
identifier_with_optional_type() {
	return Parser::spanned(identifier()) &
	       optional(Parser::token_symbol(Token::Symbol::Colon) >> Parser::spanned(type()));
}

}  // namespace AST
