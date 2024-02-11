#pragma once
#include "Function.hpp"
#include <vector>

namespace AST {

struct Program {
	std::vector<Spanned<Function>> functions;
};

Parser::Parser<Program> program();

} // namespace AST