#pragma once
#include "Function.h"
#include <vector>

namespace AST {

typedef std::vector<Function> Program;

Parser::Parser<Program> program();

} // namespace AST