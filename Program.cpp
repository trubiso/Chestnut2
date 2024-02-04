#include "Program.h"
#include "Specific.h"

namespace AST {

Parser::Parser<Program> program() {
	return Parser::transform(
	    Parser::many(Parser::trailing_semis() >> function() << Parser::trailing_semis()),
	    [](std::vector<Function> const &functions) { return Program{.functions = functions}; });
}

} // namespace AST