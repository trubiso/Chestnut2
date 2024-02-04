#pragma once
#include "Function.h"
#include <vector>

namespace AST {

struct Program {
	std::vector<Spanned<Function>> functions;
};

Parser::Parser<Program> program();

} // namespace AST