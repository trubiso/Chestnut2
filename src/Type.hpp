#pragma once
#include "Identifier.hpp"
#include <variant>

namespace AST {

struct Type {
	// TODO: BuiltIn, Generic, all composite types, ...
	enum class Kind { Identified, Inferred };

	Kind kind;
	std::variant<Identifier, std::monostate> value;

	static inline Type inferred() {
		return Type{.kind = Kind::Inferred, .value = std::monostate{}};
	}
};

Parser::Parser<Type> type_identified();
Parser::Parser<Type> type_inferred();

Parser::Parser<Type> type();

typedef std::tuple<Spanned<Identifier>, Spanned<Type>> IdentifierWithType;
Parser::Parser<IdentifierWithType> identifier_with_type();
Parser::Parser<std::tuple<Spanned<Identifier>, std::optional<Spanned<Type>>>>
identifier_with_optional_type();

} // namespace AST
