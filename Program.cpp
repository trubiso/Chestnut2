#include "Program.h"
#include "Specific.h"

namespace AST {

Parser::Parser<Program> program() {
	using namespace Parser;
	return transform(many(trailing_semis() >> spanned(function()) << trailing_semis()),
	                 [](std::vector<Spanned<Function>> const &functions) {
		                 return Program{.functions = functions};
	                 });
}

} // namespace AST