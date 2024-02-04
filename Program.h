#pragma once
#include "Function.h"
#include <vector>

namespace AST {

struct Program {
	std::vector<Function> functions;
};

Parser::Parser<Program> program();

} // namespace AST