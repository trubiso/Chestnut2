#include "Program.h"
#include "Specific.h"

namespace AST {

Parser::Parser<Program> program() {
	return Parser::many(Parser::trailing_semis() >> function() << Parser::trailing_semis());
}

} // namespace AST