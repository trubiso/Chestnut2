#pragma once
#include <vector>

#include "Function.hpp"

namespace AST {

struct Program {
	std::vector<Spanned<Function>> functions;
};

Parser::Parser<Program> program();

}  // namespace AST
