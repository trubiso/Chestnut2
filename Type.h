#pragma once
#include "Identifier.h"
#include <variant>

namespace AST {

struct Type {
	// TODO: BuiltIn, Generic, all composite types, ...
	enum class Kind { Identified, Inferred };

	Kind kind;
	std::variant<Identifier, std::monostate> value;
};

Parser::Parser<Type> type_identified();
Parser::Parser<Type> type_inferred();

Parser::Parser<Type> type();

} // namespace AST
