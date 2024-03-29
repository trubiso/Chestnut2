#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

#include "Parser.hpp"

namespace AST {

struct Identifier {
	enum class Kind {
		Unqualified,
		Qualified,
		Resolved,
		ResolvedMany,  // must resolve overload
	};

	struct QualifiedPath {
		std::vector<std::string> path;
		bool absolute;
	};

	Kind kind;
	std::variant<std::string, QualifiedPath, size_t, std::vector<size_t>> value;
};

Parser::Parser<Identifier> identifier();
Parser::Parser<Identifier> identifier_qualified();

}  // namespace AST
