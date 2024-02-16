#pragma once
#include <variant>

#include "Identifier.hpp"

namespace AST {

struct Type {
	// TODO: Generic, all composite types, ...
	enum class Kind { Identified, Inferred, BuiltIn };

	struct BuiltIn {
		enum class Kind { Int, Uint, Float, Char, String, Void } kind;
		std::optional<uint32_t> bit_width;  // bit_width is only for int/uint/float
	};

	Kind kind;
	std::variant<Identifier, BuiltIn, std::monostate> value;

	static inline Type inferred() {
		return Type{.kind = Kind::Inferred, .value = std::monostate{}};
	}
};

Parser::Parser<Type> type_identified();
Parser::Parser<Type> type_built_in();
Parser::Parser<Type> type_inferred();

Parser::Parser<Type> type();

typedef std::tuple<Spanned<Identifier>, Spanned<Type>> IdentifierWithType;
Parser::Parser<IdentifierWithType> identifier_with_type();
Parser::Parser<std::tuple<Spanned<Identifier>, std::optional<Spanned<Type>>>>
identifier_with_optional_type();

}  // namespace AST
