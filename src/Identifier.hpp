#pragma once

#include <string>
#include <variant>
#include <vector>
#include "Parser.hpp"

namespace AST {

struct Identifier {
	enum class Kind {
		Unqualified,
		Qualified,
	};

	struct QualifiedPath {
		std::vector<std::string> path;
		bool absolute;
	};

	Kind kind;
	std::variant<std::string, QualifiedPath> value;
};

Parser::Parser<Identifier> identifier();
Parser::Parser<Identifier> identifier_qualified();

} // namespace AST
